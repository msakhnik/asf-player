#include "AsfPlayer.h"

AsfPlayer::AsfPlayer()
{
    this->full_screen = false;
    this->frame_by_frame = false;
}

bool AsfPlayer::Show_Image(unsigned frame, const std::vector<int> & data_image)
{
    if (frame >= this->start_frame)
    {

        uchar *data;
        int k = 0;

        IplImage *img = cvCreateImage(cvSize(this->cols, this->rows), IPL_DEPTH_8U, 1);

        data = (uchar *) img->imageData;

        for (int i = 0; i < (img->height); i++)
        {
            for (int j = 0; j < (img->width); j++)
            {
                data[i * (img->widthStep) + j * (img->nChannels)] = data_image[k];
                k++;
            }
        }

        cvNamedWindow("frame", CV_WINDOW_NORMAL);

        if (this->full_screen)
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

        cvShowImage("frame", img);

        if (this->frame_by_frame)
        {
            cvWaitKey(0);
        }
        else
        {
            cvWaitKey(this->seconds_per_frame);
        }

        cvReleaseImage(&img);

        if (frame == this->end_frame)
        {
            cvDestroyWindow("image");
            return false;
        }
    }

    return true;
}

int AsfPlayer::Record_Video(std::vector<int> & data_image)
{
    //Шукаю будь-яку камеру
    CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
    assert(capture);

    // Виставляю розширення 320 на 240. Бо будь-яке в мене чомусь не виставляється
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 160); //1280);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 120); //960);

    double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

    IplImage* frame = 0;
    int number_frame = 1;

    cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);

        // Основні параметри для asf файла

    //Доки не натиснута кнопка ESC читаються фрейми з камери
    while (true)
    {
        //Пропуск перед фреймом. так треба для правильного зчитування

        frame = cvQueryFrame(capture);

        IplImage *dst = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
        
        //Переводжу в відтінки сірого
        cvCvtColor(frame, dst, CV_RGB2GRAY);

        for (int i = 0; i < dst->height; i++)
        {
            for (int j = 0; j < dst->width; j++)
            {
                data_image.push_back(cvGetReal2D(dst, i, j));
            }
        }

        cvShowImage("capture", dst);

        char c = cvWaitKey(33);
        //чекаю на ESC
        if (c == 27)
        { 
            break;
        }

        number_frame++;

    }
    //Звільняю пам'ять
    cvReleaseCapture(&capture);
    cvDestroyWindow("capture");
 
    return number_frame;
}