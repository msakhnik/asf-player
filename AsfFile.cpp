#include "AsfFile.h"

#include <iostream>

AsfFile::AsfFile()
{

    //initial header array
    this->info["HARDWARE"] = "0";
    this->info["HW_TYPE"] = "MOVIE";
    this->info["SENSOR_TYPE"] = "0";
    this->info["ROW_SPACING"] = "0";
    this->info["COL_SPACING"] = "0";
    this->info["SENSEL_AREA"] = "0";
    this->info["MICRO_SECOND"] = "0";
    this->info["TIME"] = "0";
    this->info["SENSITIVITY"] = "0";
    this->info["UNITS"] = "0";
    this->info["MIRROR_ROW"] = "0";
    this->info["MIRROR_COL"] = "0";
}
