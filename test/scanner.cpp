#include <iostream>
#include <list>
#include "gtest/gtest.h"
#include "common.h"

extern "C"
{
#include "../src/scanner.h"
#include "../src/vstr.h"
}

union ScannedData
{
    const char *string;
    int integer;
    Keyword keyword;
    double decimal;
};

struct ScannedToken
{
    TokenType type;
    ScannedData data;

    ScannedToken(TokenType expType)
    {
        type = expType;
        data = {.integer = 0};
    }

    ScannedToken(TokenType expType, ScannedData scannedData)
    {
        type = expType;
        data = scannedData;
    }
};

class ScannerTest : public InputTest
{
protected:
    void TestSc(std::string &input, std::list<ScannedToken> &tokens, int returnCode);
};

void ScannerTest::TestSc(std::string &input, std::list<ScannedToken> &tokens, int returnCode)
{

    std::FILE *tmpf = std::tmpfile();
    std::fputs(input.c_str(), tmpf);
    std::rewind(tmpf);

    setSourceFile(tmpf);

    Token scannedToken;
    scannedToken.type = TOKEN_EMPTY;

    auto current = tokens.begin();
    auto last = tokens.end();

    int res;
    while (1)
    {
        res = getToken(&scannedToken);
        if (scannedToken.type == TOKEN_EOF)
            break;

        if (current == last && returnCode == 0)
        {
            ASSERT_EQ(res, 0);
            continue;
        }

        if (res != 0 && returnCode != 0)
        {
            ASSERT_EQ(res, returnCode);
            break;
        }

        auto expectedToken = *current;
        ASSERT_EQ(scannedToken.type, expectedToken.type);

        switch (expectedToken.type)
        {
        case TOKEN_IDENTIFIER:
            ASSERT_STREQ(scannedToken.value.string.content, expectedToken.data.string);
            break;
        case TOKEN_STRING:
            ASSERT_STREQ(scannedToken.value.string.content, expectedToken.data.string);
            break;
        case TOKEN_KEYWORD:
            ASSERT_EQ(scannedToken.value.keyword, expectedToken.data.keyword);
            break;
        case TOKEN_INT:
            ASSERT_EQ(scannedToken.value.integer, expectedToken.data.integer);
            break;
        case TOKEN_FLOAT:
            ASSERT_DOUBLE_EQ(scannedToken.value.decimal, expectedToken.data.decimal);
            break;
        default:
            break;
        }

        current++;
    }
}

TEST_F(ScannerTest, Unknown)
{
    std::string input = "#";

    auto result = std::list<ScannedToken>{};

    TestSc(input, result, 1);
}

TEST_F(ScannerTest, Unknown2)
{
    std::string input = "189; while; #";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_INT, {.integer = 189}),
        ScannedToken(TOKEN_SEMICOLON),
        ScannedToken(TOKEN_KEYWORD, {.keyword = KW_WHILE}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 1);
}

// Scanner string tests

TEST_F(ScannerTest, StringAssignment)
{
    std::string input = "$a = \"Ahoj Kamil\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "a"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "Ahoj Kamil"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, StringAssignment2)
{
    std::string input = "$b = \"Ahoj Frajer;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "b"}),
        ScannedToken(TOKEN_ASSIGN),

    };

    TestSc(input, result, 1);
}

// Scanner math tests

TEST_F(ScannerTest, IntegerAssignment)
{
    std::string input = "$i = 256;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "i"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_INT, {.integer = 256}),
        ScannedToken(TOKEN_SEMICOLON),

    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, FloatAssignment)
{
    std::string input = "$f = 0.256;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "f"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_FLOAT, {.decimal = 0.256}),
        ScannedToken(TOKEN_SEMICOLON),

    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, Addition)
{
    std::string input = "$num = 4.5 + 6;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "num"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_FLOAT, {.decimal = 4.5}),
        ScannedToken(TOKEN_PLUS),
        ScannedToken(TOKEN_INT, {.integer = 6}),
        ScannedToken(TOKEN_SEMICOLON),

    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, Addition2)
{
    std::string input = "$num = $num + 6;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "num"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "num"}),
        ScannedToken(TOKEN_PLUS),
        ScannedToken(TOKEN_INT, {.integer = 6}),
        ScannedToken(TOKEN_SEMICOLON),

    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, Multiplication)
{
    std::string input = "$x = $y * 2;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "x"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "y"}),
        ScannedToken(TOKEN_MULTIPLY),
        ScannedToken(TOKEN_INT, {.integer = 2}),
        ScannedToken(TOKEN_SEMICOLON),

    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, Division)
{
    std::string input = "$x = $y / 2;";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "x"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "y"}),
        ScannedToken(TOKEN_DIVIDE),
        ScannedToken(TOKEN_INT, {.integer = 2}),
        ScannedToken(TOKEN_SEMICOLON),

    };

    TestSc(input, result, 0);
}

// Scanner escape sequence tests

TEST_F(ScannerTest, EscapeSequenceString)
{
    std::string input = "$a= \"Ahoj \\n Kamil\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "a"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "Ahoj \n Kamil"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, EscapeSequenceString2)
{
    std::string input = "$a= \"Kamil ma \\$wag\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "a"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "Kamil ma $wag"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, EscapeSequenceString3)
{
    std::string input = "$a= \"Ahoj\\tKamil\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "a"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "Ahoj\tKamil"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, EscapeSequenceString4) // octal notation
{
    std::string input = "$str = \"This should be empty: \\000, this is a tilde: \\176 and this is a number: \\999\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "str"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "This should be empty: \000, this is a tilde: \176 and this is a number: 999"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, EscapeSequenceString5) // hexadecimal notation
{
    std::string input = "$str = \"This should be empty: \\x00, this is a tilde: \\x7E and these are multiple characters: \\xdd\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "str"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "This should be empty: \x00, this is a tilde: \x7E and these are multiple characters: xdd"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, EscapeSequenceString6)
{
    std::string input = "$str = \"\141\x61,\\$\\x24\\n,\\x7E\\176,\\173\\x7D,\\xfj,\\999\\000\";";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_IDENTIFIER, {.string = "str"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_STRING, {.string = "aa,00,$$\n,~~,{},xfj,999000"}),
        ScannedToken(TOKEN_SEMICOLON),
    };

    TestSc(input, result, 0);
}

// Scanner loops and conditional expressions tests

TEST_F(ScannerTest, IfCondition)
{
    std::string input = "if ($a > $b) { echo \"a is greater than b\"; } else { echo \"a is lesser or equal to b\"; }";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_KEYWORD, {.keyword = KW_IF}),
        ScannedToken(TOKEN_LEFT_BRACKET),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "a"}),
        ScannedToken(TOKEN_GREATER_THAN),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "b"}),
        ScannedToken(TOKEN_RIGHT_BRACKET),
        ScannedToken(TOKEN_LEFT_BRACE),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "echo"}),
        ScannedToken(TOKEN_STRING, {.string = "a is greater than b"}),
        ScannedToken(TOKEN_SEMICOLON),
        ScannedToken(TOKEN_RIGHT_BRACE),
        ScannedToken(TOKEN_KEYWORD, {.keyword = KW_ELSE}),
        ScannedToken(TOKEN_LEFT_BRACE),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "echo"}),
        ScannedToken(TOKEN_STRING, {.string = "a is lesser or equal to b"}),
        ScannedToken(TOKEN_SEMICOLON),
        ScannedToken(TOKEN_RIGHT_BRACE),

    };

    TestSc(input, result, 0);
}

TEST_F(ScannerTest, WhileLoop)
{
    std::string input = "while ( $c !== 100 ) { $c = $c + 1; }";

    auto result = std::list<ScannedToken>{
        ScannedToken(TOKEN_KEYWORD, {.keyword = KW_WHILE}),
        ScannedToken(TOKEN_LEFT_BRACKET),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "c"}),
        ScannedToken(TOKEN_NOT_EQUAL),
        ScannedToken(TOKEN_INT, {.integer = 100}),
        ScannedToken(TOKEN_RIGHT_BRACKET),
        ScannedToken(TOKEN_LEFT_BRACE),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "c"}),
        ScannedToken(TOKEN_ASSIGN),
        ScannedToken(TOKEN_IDENTIFIER, {.string = "c"}),
        ScannedToken(TOKEN_PLUS),
        ScannedToken(TOKEN_INT, {.integer = 1}),
        ScannedToken(TOKEN_SEMICOLON),
        ScannedToken(TOKEN_RIGHT_BRACE),
    };

    TestSc(input, result, 0);
}
