#include "AsfPlayer.h"

#include <fstream>
#include <iostream>

using namespace std;

AsfPlayer::AsfPlayer(const string& file)
{
    this->filename = file;

}

bool AsfPlayer::Show_Image(unsigned frame)
{
    if (frame >= this->asf_file.start_frame)
    {

        uchar *data;
        int k = 0;

        IplImage *img = cvCreateImage(cvSize(asf_file.cols, asf_file.rows), IPL_DEPTH_8U, 1);

        data = (uchar *) img->imageData;

        for (int i = 0; i < (img->height); i++)
        {
            for (int j = 0; j < (img->width); j++)
            {
                data[i * (img->widthStep) + j * (img->nChannels)] = this->data_image[k];
                k++;
            }
        }

        this->data_image.erase(this->data_image.begin(), this->data_image.end());

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
            cvWaitKey(asf_file.seconds_per_frame);
        }

        cvReleaseImage(&img);

        if (frame == asf_file.end_frame)
        {
            cvDestroyWindow("image");
            return false;
        }
    }

    return true;
}

void AsfPlayer::Read_Line(string & str, vector<string> &array, char split)
{
    istringstream is(str);
    string s;

    while (getline(is, s, split))
        array.push_back(s);
}

bool AsfPlayer::Read_File()
{
    string line;

    string str = this->filename;

    //convert string to *char
    char * writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';

    ifstream myfile(writable);

    //flag for read data
    // if header is true - read header info
    // if header false - reaad frame data
    bool header = true;
    int frame = 0;
    int time = 0;
    int i = 0;

    if (myfile.is_open())
    {
        while (myfile.good())
        {

            getline(myfile, line);

            if (line.length() <= 3)
            {

                if (frame)
                {
                    // create and show image
                    if (!Show_Image(frame))
                        break;
                }

                //initial frame data for geting new value
                frame = 0;
                time = 0;
                header = false;

                continue;
            }

            // read video info and save in global map-array
            if (header)
            {
                vector<string> tmp_array;

                Read_Line(line, tmp_array, ' ');
                string tmp_val;

                vector<string>::iterator iter = tmp_array.begin() + 1;

                for (iter = tmp_array.begin() + 1; iter != tmp_array.end(); iter++)
                    tmp_val += *iter;

                if (tmp_array[0] == "DATA_TYPE")
                {
                    asf_file.data_type = tmp_val;
                }
                else if (tmp_array[0] == "VERSION")
                {
                    asf_file.version = tmp_val;
                }
                else if (tmp_array[0] == "NOISE_THRESHOLD")
                {
                    asf_file.noise_threshold = atoi(tmp_val.c_str());
                }
                else if (tmp_array[0] == "COLS")
                {
                    asf_file.cols = atoi(tmp_val.c_str());
                }
                else if (tmp_array[0] == "ROWS")
                {
                    asf_file.rows = atoi(tmp_val.c_str());
                }
                else if (tmp_array[0] == "START_FRAME")
                {
                    asf_file.start_frame = atoi(tmp_val.c_str());
                }
                else if (tmp_array[0] == "END_FRAME")
                {
                    asf_file.end_frame = atoi(tmp_val.c_str());
                }
                else if (tmp_array[0] == "SECONDS_PER_FRAME")
                {
                    asf_file.seconds_per_frame = 1000 * atof(tmp_val.c_str());
                }
                else if (tmp_array[0] == "ASCII_DATA")
                {
                    asf_file.ascii_data = tmp_val;
                }
                else
                {
                    asf_file.info[tmp_array[0]] = tmp_val;
                }
            }
            else
            {
                //if frame 0 then this line if title new frame.
                if (!frame)
                {

                    vector<string> tmp_array;
                    Read_Line(line, tmp_array, ',');

                    vector<string> tmp_array2;
                    Read_Line(tmp_array[0], tmp_array2, ' ');
                    frame = atoi(tmp_array2[1].c_str());

                    //if asf file without timestamp (example 1)
                    if (tmp_array.size() == 2)
                    {
                        vector<string> tmp_array2;
                        Read_Line(tmp_array[1], tmp_array2, ' ');
                        time = atoi(tmp_array2[2].c_str());
                    }

                    cout << "frame:" << frame << "  timestamp:" << time << endl;
                    i = 0;

                    continue;
                }
                else
                {
                    // read pixels info
                    vector<string> tmp_array;

                    Read_Line(line, tmp_array, ',');

                    for (unsigned j = 0; j < asf_file.cols; ++j)
                    {
                        this->data_image.push_back(atoi(tmp_array[j].c_str()));
                    }

                    i++;
                }

            }
        }
        myfile.close();
    }
    else
        return false;


    return true;
}

// show all headers

void AsfPlayer::Get_Header()
{

    AsfFile::MapType::iterator iter = asf_file.info.begin();

    cout << endl << "FILE_NAME = " << this->filename << endl;

    for (iter = asf_file.info.begin(); iter != asf_file.info.end(); iter++)
    {
        cout << "[" << (*iter).first << "] = " << (*iter).second << endl;
    }

}

bool AsfPlayer::Record_Video()
{
    string file = "example/";

    file += this->filename;
    file += ".asf";

    ofstream out(file.c_str());

    if (!out)
    {
        cout << "Cannot open file.\n";
        return false;
    }

    //Шукаю будь-яку камеру
    CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
    assert(capture);

    // Виставляю розширення 320 на 240. Бо будь-яке в мене чомусь не виставляється
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320); //1280);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240); //960);

    double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

    IplImage* frame = 0;
    int number_frame = 1;

    cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);

        // Основні параметри для asf файла

    //Перша стрічка скоріше за все неправильно. Сюди після закінчення зчитування відео я перескочу, для того щоб записати останній фрейм тут треба подумати
    out << "END_FRAME    " << endl;
    out << "DATA_TYPE " << "MOVIE" << endl;
    out << "VERSION " << "NRI Capacitive 1.0-A" << endl;
    out << "SECONDS_PER_FRAME " << "0.1" << endl;
    out << "ROWS " << height << endl;
    out << "COLS " << width << endl;
    out << "NOISE_THRESHOLD " << 0 << endl;
    out << "START_FRAME " << 1 << endl;
    out << "ASCII_DATA " << "@@" << endl;

    //Доки не натиснута кнопка ESC читаються фрейми з камери
    while (true)
    {
        //Пропуск перед фреймом. так треба для правильного зчитування
        out << endl;

        out << "Frame " << number_frame << " timestamp " << "0" << endl;

        frame = cvQueryFrame(capture);

        IplImage *dst = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
        
        //Переводжу в відтінки сірого
        cvCvtColor(frame, dst, CV_RGB2GRAY);

        for (int i = 0; i < dst->height; i++)
        {
            for (int j = 0; j < dst->width; j++)
            {
                out << cvGetReal2D(dst, i, j);
                if (j != dst->width - 1)
                    out << ",";
            }
            out << endl;
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

    //Перескакую на першу позицію і записую номер фрейма. Ну дуже не подобається, але поки не придумав нічого кращого
    out.seekp(11);
    out << number_frame << endl;
    out.close();
    
    return true;
}
