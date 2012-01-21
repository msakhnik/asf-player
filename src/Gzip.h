/* 
 * File:   cGzip.h
 * Author: morwin
 *
 * Created on 21 січня 2012, 13:02
 */

#ifndef CGZIP_H
#define	CGZIP_H

#include <stdio.h>
#include "zlib.h"

#define CHUNK 16384

class cGzip {
public:
    cGzip();
    bool Compress();
    bool Decompress();
private:

    z_stream _strm;
    int _ret;
    unsigned _have;
    unsigned char _in[CHUNK];
    unsigned char _out[CHUNK];

    void _Zerr(int);
    int _Deflate(FILE *, FILE *);
    int _Infflate(FILE *,FILE *);
};

#endif	/* CGZIP_H */

