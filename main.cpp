//
// File:   main.cpp
// Author: morwin
//
// Created on 23 листопада 2011, 11:43
//

#include "AsfFile.h"
#include "AsfPlayer.h"

#include <iostream>
#include <getopt.h>

using namespace std;

int main(int argc, char** argv)
{
    string filename = "";

    bool full_screen = false;
    bool frame_by_frame = false;

    static struct option long_options[] = {
        {"frame_by_frame", no_argument, 0, 'b'},
        {"path", required_argument, 0, 'p'},
        {"full_screen", no_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
    };

    int c = 0;
    int option_index = 0;

    c = getopt_long(argc, argv, "bfhp:",
                    long_options, &option_index);

    if (c == -1)
        return 1;

    switch (c)
    {
    case 'h':
        cout << "Help!\n"
            "-f - run on full screen\n"
            "-p - path to video file\n"
            "-b - frame by frame\n"
            "-h - help\n\n"
            "Example:\n"
            "./player -p example/example2.asf -f\n"
            "or ./player (load default video example1.asf)\n"
            "this created test.asf file into example directory"
            << endl;

        return 0;

    case 'f':
        full_screen = true;
        break;
    case 'b':
        frame_by_frame = true;
        break;
    case 'p':
        filename = optarg;
        break;
    default:
        cerr << "input error" << endl;
        break;
    }

    if (filename.empty())
        filename = "example/example1.asf";

    if (filename.length() > 4 &&
        filename.substr(filename.length() - 4, 4) == ".asf")
    {

        cAsfFile file(filename);

        cAsfPlayer player(file);

        if (!player.Init())
            return 1;

        if (full_screen)
            player.SetFullScreen(true);

        if (frame_by_frame)
            player.SetFrameByFrame(true);

        player.Play();
    }
    else
    {
        cerr << "Error syntax is incorrect!\nSyntax: " << argv[ 0 ]
            << " <file>" << endl;
        return 1;
    }

    return 0;
}

// vim: set et ts=4 sw=4:
