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

#include "CalibrationPoint.h"

const vector<double>& CalibrationPoint::getPoint() const
{
        return point;
}
vector<double>& CalibrationPoint::getData()
{
        return data;
}

void CalibrationPoint::setPoint(vector<double> Point)
{
        point=Point;
}

void CalibrationPoint::setData(vector<double> Data)
{
        data=Data;
}

CalibrationPoint::CalibrationPoint()
{

}

CalibrationPoint::CalibrationPoint(vector<double> Point, vector<double> Data)
{
        setPoint(Point);
        setData(Data);
}

CalibrationPoint::~CalibrationPoint()
{

}