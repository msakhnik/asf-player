//
// File:   main.cpp
// Author: morwin
//
// Created on 23 листопада 2011, 11:43
//

#include "AsfFile.h"
#include "AsfPlayer.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    int oc;
    string filename = "";

    bool full_screen = false;
    bool frame_by_frame = false;

    while ((oc = getopt(argc, argv, "p:hfbs")) != -1)
    {
        switch (oc)
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
    }

    if (filename.empty())
        filename = "example/example1.asf";

    // FIXME: WTF???: ./player -p asd
    if (filename.substr(filename.length() - 4, 4) == ".asf")
    {

        cAsfFile file(filename);
        cAsfPlayer player(file);

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
