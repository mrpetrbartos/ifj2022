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
