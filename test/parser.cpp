#include <iostream>
#include <list>
#include "gtest/gtest.h"
#include "common.h"

extern "C"
{
#include "../src/parser.h"
}

class ParserTest : public InputTest
{
protected:
    void TestSc(std::string &input, int returnCode);
};

void ParserTest::TestSc(std::string &input, int returnCode)
{

    std::FILE *tmpf = std::tmpfile();
    std::fputs(input.c_str(), tmpf);
    std::rewind(tmpf);

    setSourceFile(tmpf);

    if (parserInit() != 0)
        return;

    int programReturned = initialScan();

    if (returnCode != 0 && programReturned != 0)
    {
        ASSERT_EQ(returnCode, programReturned);
        return;
    }

    programReturned = parse();

    parserDestroy();

    ASSERT_EQ(returnCode, programReturned);
}

TEST_F(ParserTest, WrongPrologue)
{
    std::string input = "<?php\ndeclare(strict_types=1)";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongPrologue2)
{
    std::string input = "<?php\ndeclare(strict=1);";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongPrologue3)
{
    std::string input = "<?php\ndeclare(strict_types=0);";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongPrologue4)
{
    std::string input = "<?phpdeclare(strict_types=1);";

    TestSc(input, 1);
}

TEST_F(ParserTest, Prologue)
{
    std::string input = "<?php declare(strict_types=1);";

    TestSc(input, 0);
}

TEST_F(ParserTest, Prologue2)
{
    std::string input = "<?php\ndeclare(strict_types=1);";

    TestSc(input, 0);
}

TEST_F(ParserTest, Prologue3)
{
    std::string input = "<?php\tdeclare(strict_types=1);";

    TestSc(input, 0);
}

TEST_F(ParserTest, Prologue4)
{
    std::string input = "<?php    declare(strict_types=1);";

    TestSc(input, 0);
}

TEST_F(ParserTest, WrongEpilogue)
{
    std::string input = "<?php declare(strict_types=1); ?>a";

    TestSc(input, 1);
}

TEST_F(ParserTest, WrongEpilogue2)
{
    std::string input = "<?php declare(strict_types=1); ?>//comment";

    TestSc(input, 1);
}

TEST_F(ParserTest, WrongEpilogue3)
{
    std::string input = "<?php declare(strict_types=1); ?>$a";

    TestSc(input, 1);
}

TEST_F(ParserTest, WrongEpilogue4)
{
    std::string input = "<?php declare(strict_types=1); ?>\t";

    TestSc(input, 1);
}

TEST_F(ParserTest, Epilogue)
{
    std::string input = "<?php declare(strict_types=1); ?>";

    TestSc(input, 0);
}

TEST_F(ParserTest, Epilogue2)
{
    std::string input = "<?php declare(strict_types=1); ?>\n";

    TestSc(input, 0);
}

TEST_F(ParserTest, NoEpilogue)
{
    std::string input = "<?php declare(strict_types=1);";

    TestSc(input, 0);
}
TEST_F(ParserTest, Arithmetics1)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$a = 1.0 + 2e2 - 3.4e+9 * 4.1E-7 / 0.5;"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, Arithmetics2)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$a = 1 + 2 - 3 * 4 / 5;"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, ArithmeticsComment)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$a = 1 + 2 -/*comment in expr*/ 3 * 4 / 5;"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, Operators)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "if (1 === 1)"
        "{}"
        "else"
        "{}"
        "if (1 !== 0)"
        "{}"
        "else"
        "{}"
        "if (1 < 5)"
        "{}"
        "else"
        "{}"
        "if (1 > 0)"
        "{}"
        "else"
        "{}"
        "if (1 >= 1)"
        "{}"
        "else"
        "{}"
        "if (1 <= 1)"
        "{}"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, Concatenate)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$var = \" a \" . \" b \";"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, WrongExpression)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$var = 1 * 2 2;"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongOperator)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$var = 1 ** 2;"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongBrackets)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$a = (((1 + 2) - ((3) * 4)) / 5));"
        "$b = (\" a \" . \" b \";"
        "if (1 === (1)"
        "{}"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, Brackets)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$a = (((1 + 2) - ((3) * 4)) / 5);"
        "$b = (\" a \") . (\" b \");"
        "if (1 === (1))"
        "{}"
        "else"
        "{}"
        "if ((1 !== 0))"
        "{}"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, Variables)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$varA = 1;"
        "$varB = 2;"
        "$varC = 3;"
        "$varD = 4;"
        "$varE = 5;"
        "$result = $varA + ($varB - $varC) * $varD / ($varE);"
        "return 0;";

    TestSc(input, 0);
}

TEST_F(ParserTest, AssignmentExample1)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$a = readi();"
        "write(\"Zadejte cislo pro vypocet faktorialu\\n\");"
        "if ($a === null)"
        "{"
        "write(\"Chyba pri nacitani celeho cisla!\\n\");"
        "}"
        "else"
        "{"
        "}"
        "if ($a < 0)"
        "{"
        "write(\"Faktorial nelze spocitat\\n\");"
        "}"
        "else"
        "{"
        "$vysl = 1;"
        "while ($a > 0)"
        "{"
        "$vysl = $vysl * $a;"
        "$a = $a - 1;"
        "}"
        "write(\"Vysledek je: \", $vysl, \"\\n\");"
        "}";

    TestSc(input, 0);
}

TEST_F(ParserTest, AssignmentExample2)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "write(\"Zadejte cislo pro vypocet faktorialu: \");"
        "$a = readi();"
        "function factorial(int $n) : int {"
        "if ($n < 2) {"
        "$result = 1;"
        "} else {"
        "$decremented_n = $n - 1;"
        "$temp_result = factorial($decremented_n);"
        "$result = $n * $temp_result;"
        "}"
        "return $result;"
        "}"
        "if ($a !== null) {"
        "if ($a < 0)	{"
        "write(\"Faktorial nelze spocitat\\n\");"
        "} else {"
        "$vysl = factorial($a);"
        "write(\"Vysledek je: \", $vysl, \"\\n\");"
        "}"
        "} else {"
        "write(\"Chyba pri nacitani celeho cisla!\\n\");"
        "}";

    TestSc(input, 0);
}

TEST_F(ParserTest, AssignmentExample3)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "$str1 = \"Toto je nejaky text v programu jazyka IFJ22\";"
        "$str2 = $str1 . \", ktery jeste trochu obohatime\";"
        "write($str1, \"\\n\", $str2, \"\\n\");"
        "write(\"Pozice retezce \\\"text\\\" v \\$str2: \", 15, \"\\n\");"
        "write(\"Zadejte serazenou posloupnost vsech malych pismen a-h, \");"
        "$str1 = reads();"
        "if ($str1 !== null) {"
        "while ($str1 !== \"abcdefgh\") {"
        "write(\"Spatne zadana posloupnost, zkuste znovu:\\n\");"
        "$str1 = reads();"
        "}"
        "} else { }"
        "?>";

    TestSc(input, 0);
}

TEST_F(ParserTest, WrongFunction1)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "function fun () : void"
        "{}"
        "fun;"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongFunction2)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "function fun () : void"
        "{}"
        "fun(;"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongFunction3)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "function fun () : void"
        "{}"
        "function fun();"
        "return 0;";

    TestSc(input, 3);
}

TEST_F(ParserTest, WrongFunction4)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "function fun () : int"
        "{"
        "return 0;"
        "}"
        "$var fun();"
        "return 0;";

    TestSc(input, 5);
}

TEST_F(ParserTest, WrongFunction5)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "function fun () : int"
        "{"
        "return 0;"
        "}"
        "$var = ();"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongIf1)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "if (1 === 1)"
        "$var = 0;"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongIf2)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "if (1 === 1)"
        "{}"
        "else"
        "$var = 0;"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongIf3)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "if"
        "{}"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongIf4)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "if ()"
        "{}"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongIf5)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "if ("
        "{}"
        "else"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongReturn)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "return string;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongWhile1)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "while (1===1)"
        "$var = 0;"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongWhile2)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "while"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongWhile3)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "while ()"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, WrongWhile4)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "while )"
        "{}"
        "return 0;";

    TestSc(input, 2);
}

TEST_F(ParserTest, While)
{
    std::string input =
        "<?php\n"
        "declare(strict_types=1);"
        "while (1===0)"
        "{"
        "while (7)"
        "{}"
        "}"
        "return 0;";

    TestSc(input, 0);
}