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
            cout << "Help!" << endl;
            cout << "-f - run on full screen" << endl;
            cout << "-p - path to video file" << endl;
            cout << "-b - frame by frame" << endl;
            cout << "-h - help" << endl;
            cout << endl << "Example:" << endl;
            cout << "./player -p example/example2.asf -f" << endl;
            cout << "or ./player (load default video example1.asf)" << endl;
            cout << "this created test.asf file into example directory"
                << endl;

            return 0;

            break;
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

    if (filename.length() == 0)
        filename = "example/example1.asf";

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
        exit(1);
    }

    return 0;
}
