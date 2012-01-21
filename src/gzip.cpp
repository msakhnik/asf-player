//
// gzip.cpp
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

#include "Gzip.h"
#include "zlib.h"

#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

using namespace std;

static char const* _Basename(char const* fname)
{
    char const* res = strrchr(fname, '/');
    return res ? res + 1 : fname;
}

static void _ReadHelp(const char *progname)
{
    cout << "Gzip tool\n\n"
        "Synopsis:\n"
        "  " << progname << " [options] < source > dest\n\n"
        "Options:\n"
        "  -c,--compress\t\tCompressed source in dest\n"
        "  -d,--decompress\tDecompressed from source in dest\n"
        "  -h,--help\t\tThis help message\n\n"
        "Example:\n"
        "  " << progname << " -d < example.asf.gz > foo.asf \n"
        "  this decompressed example.asf.gz in foo.asf\n"
        "  " << progname << " -c < example.asf > foo.asf.gz \n"
        "  compressed example.asf and create foo.asf.gz"
        << endl;
}

int main(int argc, char** argv)
{
    cGzip gz;
    char const* progname = _Basename(argv[0]);

    if (argc < 2)
    {
        cerr << "Unknown input data (try --help)" << endl;
        return 1;
    }

    while (true)
    {
        static struct option long_options[] ={
            { "compress", no_argument, 0, 'c'},
            { "decompress", no_argument, 0, 'd'},
            { "help", no_argument, 0, 'h'},
            { 0, 0, 0, 0}
        };

        int c = 0;
        int option_index = 0;

        c = getopt_long(argc, argv, "cdh",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'h':
            _ReadHelp(progname);
            return 0;

        case 'c':
            if (!gz.Compress())
                return 1;
            break;

        case 'd':
            if (!gz.Decompress())
                return 1;
            break;
        default:
            _ReadHelp(progname);
            return 1;
        }
    }


    return 0;
}

// vim: set et ts=4 sw=4:
