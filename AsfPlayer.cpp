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
    if (!w || !h)
    {
        cerr << "Can't reproduce one-dimensional film" << endl;
        return false;
    }

    // Перевіряю ширину висоту і чи _img не NULL
    _img = cvCreateImage(cvSize(_file.GetCols(), _file.GetRows()),
                         IPL_DEPTH_8U, 1);

    if (!_img)
    {
        cerr << "Cannot create a window " << endl;
        return false;
    }

    // static_cast працює тільки між сумісними типами. Якщо ж треба
    // взяти грубо, без reinterpret_cast не обійтися.
    _data = reinterpret_cast<uchar *>(_img->imageData);

    cvNamedWindow("frame", CV_WINDOW_NORMAL);

    return true;
}

bool cAsfPlayer::Play()
{
    //Читаю і показую по кадрові
    if (this->_ShowFrame())
        return true;

    cerr << "Error reading frames" << endl;
    return false;
}

bool cAsfPlayer::_ShowFrame()
{
    //Отримую перший кадр
    if (!_file.ReadFrame())
        return false;

    //Ініціалізація вікна для відтворення даних

    unsigned int start = this->_file.GetStartFrame();
    unsigned int end = this->_file.GetEndFrame();

    for (unsigned int frame = start;
        frame <= end; ++frame)
    {
        cout << "Frame: " << frame << endl;

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

        if (this->_full_screen)
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                                CV_WINDOW_FULLSCREEN);

        cvShowImage("frame", _img);

        //Тут запам'ятовую час для зчитування наступного кадра
        struct timeval t0;
        if (!_frame_by_frame)
            gettimeofday(&t0, NULL);

        // FIXME: Check if the frame was read successfully.
        if (!_file.ReadFrame() && frame != end)
        {
            cerr << "Some data is lost" << endl;
        }

        // How many milliseconds to wait while the frame is being exposed
        int wait_time (0);

        struct timeval t1;
        if (!_frame_by_frame)
        {
            gettimeofday(&t1, NULL);
            int read_time = (t1.tv_usec - t0.tv_usec) / 1000;
            //Якщо зчитувалось довше ніж seconds_per_frame (затримка перед
            //наступним кадром) то виводжу повідомлення про повільне зчитування
            //інакше віднімаю отриманий час
            // FIXME: SecondsPerFrame are milliseconds indeed?
            wait_time = _file.GetSecondsPerFrame() - read_time;
            if (wait_time < 0)
            {
                cout << "Slow playing..." << endl;
                // FIXME: Can we put 0 here?
                wait_time = 1;
            }
        }

        // User input
        int key = cvWaitKey(wait_time);
        if ('q' == key)
        {
            cout << "Bye!" << endl;
            return true;
        }
    }

    return true;
}

// vim: set et ts=4 sw=4:
