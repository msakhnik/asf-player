#include "AsfPlayer.h"
#include "AsfFile.h"

#include <iostream>
#include <sys/time.h>

using namespace std;

cAsfPlayer::cAsfPlayer(cAsfFile &file)
    : _file(file)
    , _full_screen(false)
    , _frame_by_frame(false)
{
}

cAsfPlayer::~cAsfPlayer()
{
    cvReleaseImage(&_img);
    cvDestroyWindow("image");
}

bool cAsfPlayer::Init()
{
    if (!_file.ReadHeader())
    {
        cerr << "Error reading from file" << endl;
        return false;
    }

    int w = _file.GetCols();
    int h = _file.GetRows();

    //check the width and height
    if (!w || !h)
    {
        cerr << "Can't reproduce one-dimensional film" << endl;
        return false;
    }

    _img = cvCreateImage(cvSize(_file.GetCols(), _file.GetRows()),
                         IPL_DEPTH_8U, 1);

    if (!_img)
    {
        cerr << "Cannot create a window " << endl;
        return false;
    }

    _data = reinterpret_cast<uchar *>(_img->imageData);

    cvNamedWindow("frame", CV_WINDOW_NORMAL);

    return true;
}

bool cAsfPlayer::Play()
{
    if (this->_ShowFrame())
    {
        cout << endl;
        return true;
    }

    cerr << "Error reading frames" << endl;
    return false;
}

bool cAsfPlayer::_ShowFrame()
{
    //Get first frame
    if (!_file.ReadFrame())
        return false;

    unsigned int start = this->_file.GetStartFrame();
    unsigned int end = this->_file.GetEndFrame();

    for (unsigned int frame = start;
        frame <= end; ++frame)
    {
        cout << "\rFrame: " << frame << flush;

        cAsfFile::FrameT const& image_data = _file.GetLastFrame();
        vector<int>::const_iterator iter = image_data.begin();

        for (int i = 0; i < _img->height; ++i)
        {
            for (int j = 0; j < _img->width; ++j)
            {
                _data[i * _img->widthStep + j * _img->nChannels]
                    = *(iter++);
            }
        }
        //Check on full screen mode
        if (this->_full_screen)
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                                CV_WINDOW_FULLSCREEN);

        cvShowImage("frame", _img);

        //It remembers the moment  to read the next frame
        struct timeval t0;
        if (!_frame_by_frame)
            gettimeofday(&t0, NULL);

        if (!_file.ReadFrame() && frame != end)
        {
            cerr << "\nSome data is lost" << endl;
        }

        // How many milliseconds to wait while the frame is being exposed
        int wait_time (0);

        struct timeval t1;
        if (!_frame_by_frame)
        {
            gettimeofday(&t1, NULL);
            int read_time = (t1.tv_usec - t0.tv_usec) / 1000;

            wait_time = _file.GetMsecPerFrame() - read_time;

            if (wait_time <= 0)
            {
                cout << "\nSlow playing..." << endl;
                wait_time = 1;
            }
        }

        // User input
        int key = cvWaitKey(wait_time);
        if ('q' == key)
        {
            cout << "\nBye!" << endl;
            return true;
        }
    }

    return true;
}

// vim: set et ts=4 sw=4:
