//
// AsfFile.h
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

#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>

class cAsfFile
{
public:

    cAsfFile(const std::string&);

    // read from file
    bool ReadHeader();

    typedef std::vector<int> FrameT;
    void ReadFrame();
    FrameT const& GetLastFrame() const { return _last_frame; }
    bool SetPositionFrame();
    void ChangePosition(unsigned int);

    std::string const& GetDataType() const { return _data_type; }
    std::string const& GetVersion() const { return _version; }
    double GetMsecPerFrame() const { return _seconds_per_frame; }
    unsigned int GetRows() const { return _rows; }
    unsigned int GetCols() const { return _cols; }
    int GetNoiseThreshold() const { return _noise_threshold; }
    unsigned int GetStartFrame() const { return _start_frame; }
    unsigned int GetEndFrame() const { return _end_frame; }
    std::string const& GetAsciiData() const { return _ascii_data; }
    std::string const& GetInfo(std::string & key) { return _info[key]; }

private:
    std::ifstream _file;

    typedef std::map<std::string, std::string>
        _KeyValT;

    _KeyValT _info;

    std::string _data_type;
    std::string _version;
    double _seconds_per_frame;
    unsigned int _rows;
    unsigned int _cols;
    int _noise_threshold;
    unsigned int _start_frame;
    unsigned int _end_frame;
    std::string _ascii_data;

    // We keep last frame to avoid constant memory allocation.
    FrameT _last_frame;

    std::vector<unsigned long int> _frame_position;
};

// vim: set et ts=4 sw=4:
