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
