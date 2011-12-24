//
// File:   AsfFile.h
// Author: morwin
//
// Created on 26 листопада 2011, 17:35
//

#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>

class cAsfFile {
public:

    cAsfFile(const std::string&);

    // read from file
    void ReadHeader();
    std::vector<int> ReadFrame();

    typedef std::map<std::string, std::string>
        MapType;

    void SetDataType(std::string & value){_data_type = value; };
    void SetVersion(std::string & value) {_version = value; };
    void SetSecondsPerFrame(double value) {_seconds_per_frame = value; };
    void SetRows(unsigned int value) {_rows = value; };
    void SetCols(unsigned int value) {_cols = value; };
    void SetNoiseThreshold(int value) {_noise_threshold = value; };
    void SetStartFrame(unsigned int value) {_start_frame = value; };
    void SetEndFrame(unsigned int value) {_end_frame = value; };
    void SetAsciiData(std::string & value) {_ascii_data = value; };
    void SetInfo(std::string key, std::string value) {_info[key] = value; };

    std::string GetDataType() const {return _data_type;};
    std::string GetVersion() const {return _version;};
    double GetSecondsPerFrame() const {return _seconds_per_frame;};
    unsigned int GetRows() const {return _rows;};
    unsigned int GetCols() const {return _cols;};
    int GetNoiseThreshold() const {return _noise_threshold;};
    unsigned int GetStartFrame() const {return _start_frame;};
    unsigned int GetEndFrame() const {return _end_frame;};
    std::string GetAsciiData() const {return _ascii_data;};
    std::string GetInfo(std::string & key) {return _info[key]; };

private:
    std::string _filename;
    std::ifstream _file;

    MapType _info;

    std::string _data_type;
    std::string _version;
    double _seconds_per_frame;
    unsigned int _rows;
    unsigned int _cols;
    int _noise_threshold;
    unsigned int _start_frame;
    unsigned int _end_frame;
    std::string _ascii_data;
};
