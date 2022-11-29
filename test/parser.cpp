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
