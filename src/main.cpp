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

static char const* _Basename(char const* fname)
{
    char const* res = strrchr(fname, '/');
    return res ? res + 1 : fname;
}

static void _ReadHelp(const char * progname)
{
    cout << "ASF video player\n\n"
                "Synopsis:\n"
                "  " << progname << " [options]\n"
                "  " << progname <<" [options] [file]\n\n"
                "Options:\n"
                "  -f,--full-screen\tPlay on full screen\n"
                "  -p,--path\t\tPath to video file\n"
                "  -b,--frame-by-frame\tShow frame by frame\n"
                "  -h,--help\t\tThis help message\n\n"
                "Example:\n"
                "  " << progname << " example/example1.asf\n"
                "  " << progname << " -p example/example2.asf -f\n"
                "  or " << progname << " (load default video example1.asf)\n"
                "  this creates a file test.asf in the directory 'example'"
                << endl;
}

int main(int argc, char** argv)
{
    char const* progname = _Basename(argv[0]);
    string filename;

    bool full_screen = false;
    bool frame_by_frame = false;

     if(argc == 1)
     {
         _ReadHelp(progname);
         return 0;
     }

    int counter = 0;
    while (1)
    {
        ++counter;
        static struct option long_options[] =
        {
            { "frame-by-frame", no_argument,        0, 'b' },
            { "path",           required_argument,  0, 'p' },
            { "full-screen",    no_argument,        0, 'f' },
            { "help",           no_argument,        0, 'h' },
            { 0, 0, 0, 0 }
        };

        int c = 0;
        int option_index = 0;

        c = getopt_long(argc, argv, "bfhp:",
                        long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 'h':
            _ReadHelp(progname);
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

    if (argc != counter && filename.empty())
    {
        filename = argv[argc - 1];
        cout << filename << endl;
    }

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
