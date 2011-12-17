//
// File:   main.cpp
// Author: morwin
//
// Created on 23 листопада 2011, 11:43
//

#include "AsfFile.h"

#include <string>
#include <cstdlib>

#include <stdio.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{

    int oc;
    string filename = "";

    bool full_screen = false;
    bool frame_by_frame = false;
    bool show_headers = false;

    while ((oc = getopt(argc, argv, "p:hfbsw:")) != -1)
    {
        switch (oc)
        {
        case 'h':
            cout << "Help!" << endl;
            cout << "-f - run on full screen" << endl;
            cout << "-p - path to video file" << endl;
            cout << "-b - frame by frame" << endl;
            cout << "-s - show asf info " << endl;
            cout << "-w - write asf file " << endl;
            cout << "-h - help" << endl;
            cout << endl << "Example:" << endl;
            cout << "./player -p example/example2.asf -f" << endl;
            cout << "or ./player (load default video example1.asf)" << endl;
            cout << "For record video run like this:" << endl;
            cout << "./player -w test" << endl;
            cout << "this created test.asf file into example directory" << endl;

            return 0;

            break;
        case 'f':
            full_screen = true;
            break;
        case 'b':
            frame_by_frame = true;
            break;
        case 's':
            show_headers = true;
            break;
        case 'p':
            filename = optarg;
            break;
        case 'w':
            if (optarg)
            {
                AsfFile rec(optarg);
                
                if (!rec.Write_File()) {
                    cerr << "Failed to read data" << endl;
                    exit(0);
                }
                
                return 0;

            }
            break;
        default:
            cout << "input error" << endl;
            ;
            break;
        }
    }

    if (filename.length() == 0)
        filename = "example/example1.asf";

    if (filename.substr(filename.length() - 4, 4) == ".asf")
    {
        
        AsfFile play(filename);

        if (full_screen)
            play.Set_Full_Screen();

        if (frame_by_frame)
            play.Set_Frame_By_Frame();
        
        if (!play.Read_File())
        {
            cerr << "Failed to read data" << endl;
            exit(0);
        }

        if (show_headers)
            play.Get_Header();

    }
    else
    {
        cerr << "Error syntax is incorrect!\nSyntax: " << argv[ 0 ] << " <file>\n";
        exit(1);
    }

    return 0;
}
