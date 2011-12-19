//
// File:   main.cpp
// Author: morwin
//
// Created on 23 листопада 2011, 11:43
//

#include "cAsfFile.h"
#include "cAsfPlayer.h"

#include <string>
#include <cstdlib>

#include <stdio.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    cAsfFile file("example/example1.asf");
    cAsfPlayer player(file);
    player.Play();

    return 0;
}
