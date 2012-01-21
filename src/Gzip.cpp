/* 
 * File:   cGzip.cpp
 * Author: morwin
 * 
 * Created on 21 січня 2012, 13:02
 */

#include "Gzip.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

using namespace std;

cGzip::cGzip() : _ret(0)
{
    _strm.zalloc = Z_NULL;
    _strm.zfree = Z_NULL;
    _strm.opaque = Z_NULL;
    _strm.avail_in = 0;
    _strm.next_in = Z_NULL;
}

int cGzip::_Deflate(FILE *source, FILE *dest)
{
    int flush = 0;
    do
    {
        _strm.avail_in = fread(_in, 1, CHUNK, source);
        if (ferror(source))
            return Z_ERRNO;
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        _strm.next_in = _in;
        do
        {
            _strm.avail_out = CHUNK;
            _strm.next_out = _out;
            _ret = deflate(&_strm, flush);
            assert(_ret != Z_STREAM_ERROR);
            _have = CHUNK - _strm.avail_out;
            if (fwrite(_out, 1, _have, dest) != _have || ferror(dest))
            {
                return Z_ERRNO;
            }
        }
        while (_strm.avail_out == 0);
        assert(_strm.avail_in == 0);
    }
    while (flush != Z_FINISH);
    assert(_ret == Z_STREAM_END);
    return Z_OK;
}

int cGzip::_Infflate(FILE *source, FILE *dest)
{
    do
    {
        _strm.avail_in = fread(_in, 1, CHUNK, source);
        if (ferror(source))
            return Z_ERRNO;
        if (_strm.avail_in == 0)
            break;
        _strm.next_in = _in;
        do
        {
            _strm.avail_out = CHUNK;
            _strm.next_out = _out;
            _ret = inflate(&_strm, Z_NO_FLUSH);
            assert(_ret != Z_STREAM_ERROR);
            switch (_ret)
            {
                case Z_NEED_DICT:
                    _ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    return _ret;
            }
            _have = CHUNK - _strm.avail_out;
            if (fwrite(_out, 1, _have, dest) != _have || ferror(dest))
                return Z_ERRNO;
        }
        while (_strm.avail_out == 0);
    }
    while (_ret != Z_STREAM_END);
    return _ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

void cGzip::_Zerr(int _ret)
{
    cerr << "Error:" << endl;

    switch (_ret)
    {
    case Z_ERRNO:
        if (ferror(stdin))
            cerr << "error reading stdin" << endl;
        if (ferror(stdout))
            cerr << "error writing stdout" << endl;
        break;
    case Z_STREAM_ERROR:
        cerr << "invalid compression level" << endl;
        break;
    case Z_DATA_ERROR:
        cerr << "invalid or incomplete deflate data" << endl;
        break;
    case Z_MEM_ERROR:
        cerr << "_out of memory" << endl;
        break;
    case Z_VERSION_ERROR:
        cerr << "zlib version mismatch!" << endl;
    }
}

bool cGzip::Compress()
{
    _ret = deflateInit(&_strm, 1);

    if (_ret == Z_OK)
    {
        _ret = _Deflate(stdin, stdout);
        deflateEnd(&_strm);
    }

    if (_ret != Z_OK)
    {
        _Zerr(_ret);
        return false;
    }
    else
        return true;
}

bool cGzip::Decompress()
{
    _ret = inflateInit(&_strm);

    if (_ret == Z_OK)
    {
        _ret = _Infflate(stdin, stdout);
        inflateEnd(&_strm);
    }

    if (_ret != Z_OK)
    {
        _Zerr(_ret);
        return false;
    }
    else
        return true;
}


