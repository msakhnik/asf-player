/* 
 * File:   main.cpp
 * Author: morwin
 *
 * Created on 8 березня 2012, 17:23
 */

#include <cstdlib>
#include<gtest/gtest.h>
#include "../src/AsfFile.h"
#include "../src/AsfPlayer.h"

using namespace std;

const string filename = "../example/example1.asf";
cAsfFile file(filename);
cAsfPlayer player(file);
////////////////////////////////////////////////////////
//////////////////// cAsfFile //////////////////////
////////////////////////////////////////////////////////
TEST(AsfFileTest, ReadHeader)
{
    ASSERT_TRUE(file.ReadHeader());
}

TEST(AsfFileTest, ReadFrame)
{
    ASSERT_TRUE(file.ReadFrame());
}

TEST(AsfFileTest, RowsAndCols)
{
    ASSERT_GT(file.GetCols(), 0);
    ASSERT_GT(file.GetRows(), 0);
}

////////////////////////////////////////////////////////
//////////////////// cAsfPlayer //////////////////
////////////////////////////////////////////////////////

TEST(AsfPlayerTest, Init)
{
    ASSERT_TRUE(player.Init());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
