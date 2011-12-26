#include "AsfPlayer.h"
#include "AsfFile.h"

#include <iostream>
#include <sys/time.h>

using namespace std;

cAsfPlayer::cAsfPlayer(cAsfFile &file)
    : _file(file)
    , _full_screen(false)
    , _frame_by_frame(false)
{}

cAsfPlayer::~cAsfPlayer()
{
    cvReleaseImage(&_img);
    cvDestroyWindow("image");
}

// Це мабуть не правильне рішення створювати функцію для ініціалізації.
// Адже втрачається сама ідея конструктора
bool cAsfPlayer::Init()
{
    if (!_file.ReadHeader())
    {
        cerr << "Error reading from file" << endl;
        return false;
    }

    int w = _file.GetCols();
    int h = _file.GetRows();

    // FIXME: What if it can't create a window of the requested size?
    // Перевіряю ширину висоту і чи _img не NULL

    _img = cvCreateImage(cvSize(_file.GetCols(), _file.GetRows()),
                         IPL_DEPTH_8U, 1);

    if (!_img && !w && !h)
    {
        cerr << "Cannot create a window " << endl;
        return false;
    }

    // FIXME: Use C++-style cast: static_cast vs reinterpret_cast?
    // Я так зрозумів, що для стандартних типів потрібно використовувати
    //static_cast
    //а для типів створених користувачем, де вся вина лягає на плечі
    // програміста - reinterpret_cast.
    _data = reinterpret_cast<uchar *>(_img->imageData);

    cvNamedWindow("frame", CV_WINDOW_NORMAL);

    return true;
}

bool cAsfPlayer::Play()
{
    //Читаю і показую по фреймові
    if (this->_ShowFrame())
        return true;

    cerr << "Error reading frames" << endl;
    return false;
}

bool cAsfPlayer::_ShowFrame()
{
    //Отримую перший фрейм
    if (!_file.ReadFrame())
        return false;

    //Ініціалізація вікна для відтворення даних

    unsigned int start = this->_file.GetStartFrame();
    unsigned int end = this->_file.GetEndFrame();

    for (unsigned int frame = start;
        frame <= end; ++frame)
    {
        cout << "Frame: " << frame << endl;

        // FIXME: What is this k?

        cAsfFile::FrameT const& image_data = _file.GetLastFrame();
        vector<int>::const_iterator iter = image_data.begin();

        for (int i = 0; i < _img->height; ++i)
        {
            for (int j = 0; j < _img->width; ++j)
            {
                // FIXME: Is this correct? We can clearly see that the
                // leftmost columns repeat the right part of a frame.
                //Дуже цікава помилка була =)
                //Виявляється дані неправильно зчитувались
                //Зчитувалась сама назва фрейма
                //якось конвертувалась і спричиняла зсув
                //всіх елементів

                _data[i * _img->widthStep + j * _img->nChannels]
                    = *(iter++);
            }
        }

        // FIXME: Don't use accessors for the class's own members.
        // That's ridiculous.
        if (this->_full_screen)
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                                CV_WINDOW_FULLSCREEN);

        cvShowImage("frame", _img);

        //Тут запам'ятовую час для зчитування наступного кадра
        struct timeval t0;
        if (!this->_frame_by_frame)
            gettimeofday(&t0, NULL);

        // FIXME: Check if the frame was read successfully.
        if (!_file.ReadFrame() && frame != end)
        {
            cerr << "Some data is lost" << endl;
        }

        if (!this->_frame_by_frame)
        {
            struct timeval t1;
            gettimeofday(&t1, NULL);

            int time = (double) (t1.tv_usec - t0.tv_usec) / 1000;

            //Якщо зчитувалось довше ніж seconds_per_frame (затримка перед
            //наступним кадром) то виводжу повідомлення про повільне зчитування
            //інакше віднімаю отриманий час
            if (time >= this->_file.GetSecondsPerFrame())
            {
                cout << "Slow playing..." << endl;
            }
            else
            {
                cvWaitKey(this->_file.GetSecondsPerFrame() - time);
            }
        }
        else
            cvWaitKey(0);
    }

    return true;
}
