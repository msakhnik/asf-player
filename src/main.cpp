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

static void _ReadHelp(const char *progname)
{
    cout << "ASF video player\n\n"
            "Synopsis:\n"
            "  " << progname <<" [options] file\n\n"
            "Options:\n"
            "  -f,--full-screen\tPlay on full screen\n"
            "  -b,--frame-by-frame\tShow frame by frame\n"
            "  -s,--scale\t\tSet scale\n"
            "  -t,--track\t\tSet trackbar\n"
            "  -h,--help\t\tThis help message\n\n"
            "Example:\n"
            "  " << progname << " example/example1.asf\n"
            "  " << progname << " -f example/example2.asf\n"
            "  " << progname << " -s2 example/example3.asf\n"
            "  this creates a file test.asf in the directory 'example'"
         << endl;
}

int main(int argc, char** argv)
{
    char const* progname = _Basename(argv[0]);

    bool full_screen = false;
    bool frame_by_frame = false;
    unsigned int scale = 1;
    bool track = false;

    while (true)
    {
        static struct option long_options[] =
        {
            { "frame-by-frame", no_argument,    0, 'b' },
            { "full-screen",    no_argument,        0, 'f' },
            { "scale",          required_argument, 0, 's' },
            { "track",          no_argument,          0, 't' },
            { "help",           no_argument,           0, 'h' },
            { 0, 0, 0, 0 }
        };

        int c = 0;
        int option_index = 0;

        c = getopt_long(argc, argv, "bfs:th",
                        long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 'h':
            _ReadHelp(progname);
            return 0;

        case 't':
            track = true;
            break;

        case 's':
            scale = atoi(optarg);
            if (scale < 1 || scale > 20)
            {
                cerr << "Scale is not valid! Only 1..20 range" << endl;
                return 1;
            }
            break;

        case 'f':
            full_screen = true;
            break;

        case 'b':
            frame_by_frame = true;
            break;

        default:
            cerr << "Unknown option '" << c << "'" << endl;
            return 1;
        }
    }

    if (argc != optind + 1)
    {
        cerr << "File name is missing (try --help)" << endl;
        return 1;
    }

    string filename = argv[optind];
    cout << filename << endl;

    if (filename.length() < 4 ||
        filename.substr(filename.length() - 4, 4) != ".asf")
    {
        cerr << "File '" << filename << "' isn't supported" << endl;
        return 1;
    }

    cAsfFile file(filename);

    cAsfPlayer player(file);

    if (full_screen)
        player.SetFullScreen(true);
    else
    {
        if (scale != 1)
            player.SetScale(scale);
    }

    if (frame_by_frame)
        player.SetFrameByFrame(true);

    if (track)
        player.SetTrack(true);

    if (!player.Init())
        return 1;

    player.Play();

    return 0;
}

// vim: set et ts=4 sw=4:
