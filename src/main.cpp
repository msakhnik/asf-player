//
// main.cpp
//
//     Created: 02.12.2012
//      Author: Sakhnik
//
// This file is part of Asf Player.
//
// Asf Player is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Asf Player License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Asf Player is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Asf Player.  If not, see <http://www.gnu.org/licenses/>

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
            "  -r,--record\t\tRecord video from webcam\n"
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
    unsigned int scale = 1;
    bool record = false;
    string filename;
    while (true)
    {
        static struct option long_options[] =
        {
            { "scale",          required_argument, 0, 's' },
            { "record",        required_argument,          0, 'r' },
            { "help",           no_argument,           0, 'h' },
            { 0, 0, 0, 0 }
        };

        int c = 0;
        int option_index = 0;

        c = getopt_long(argc, argv, "fr:s:th",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'h':
            _ReadHelp(progname);
            return 0;

        case 'r':
            record = true;
            filename = optarg;
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

        default:
            cerr << "Unknown option '" << c << "'" << endl;
            return 1;
        }
    }

    if (argc != optind + 1 && !record)
    {
        cerr << "File name is missing (try --help)" << endl;
        return 1;
    }
    if (filename.length() == 0)
        filename = argv[optind];

    if (filename.length() < 4 ||
        filename.substr(filename.length() - 4, 4) != ".asf")
    {
        cerr << "File '" << filename << "' isn't supported" << endl;
        return 1;
    }

    cAsfFile file(filename);

    cAsfPlayer player(file);
    if (record)
    {
        player.RecordVideo();
        return 0;
    }

    if (full_screen)
        player.SetFullScreen(true);
    else
    {
        if (scale != 1)
            player.SetScale(scale);
    }


    if (!player.Init())
        return 1;

    player.Play();

    return 0;
}

// vim: set et ts=4 sw=4:
