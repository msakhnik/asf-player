//
// File:   main.cpp
// Author: morwin
//
// Created on 23 листопада 2011, 11:43
//

#include "AsfFile.h"
#include "AsfPlayer.h"

using namespace std;

int main(int argc, char** argv)
{
    cAsfFile file("example/example2.asf");
    cAsfPlayer player(file);
    player.Play();

    return 0;
}
