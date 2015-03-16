/* OscCalibrator - A mapping and routing tool for use with the Open Sound Control protocol.
   Copyright (C) 2012  Dionysios Marinos - fewbio@googlemail.com

   This program is free software: you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software Foundation,
   either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.
   You should have received a copy of the GNU General Public License along with this program.
   If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <vector>
using namespace std;

class CalibrationPoint
{
        vector<double> point;
        vector<double> data;

public:

  const vector<double>& getPoint() const;
        vector<double>& getData();

        void setPoint(vector<double> Point);
        void setData(vector<double> Data);

        CalibrationPoint();
        CalibrationPoint(vector<double> Point, vector<double> Data);
        ~CalibrationPoint();
};