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

#include "Simplex.h"

struct ExtrapolationResult
{
	vector<double> values;
	double weight;
	bool onRidge;
};

class QhullCalibrator
{
	qhT* qHullContext;
	qhT* qHullConvexContext;
	FILE errFile;

	double* points;
	vector<Simplex*> simplices;
	void performTriangulation();

	int spaceDimension;
	int dataDimension;
	vector<CalibrationPoint> calibrationPoints;
	vector<double> lastInterpolationResult;

	double distance(CalibrationPoint A, CalibrationPoint B);
	vector<CalibrationPoint> getSimplexPoints(vector<double> Point);

	int inverse(double **a, double **ainv, int n);

	static bool calibrationPointSortPredicate(const CalibrationPoint& a, const CalibrationPoint& b);

	//3d routines
	vector<double> crossProduct(vector<double> a, vector<double> b);
	double dotProduct(vector<double> a, vector<double> b);

	//Misc routines
	vector<int> getIndicesOfCalibrationPoints(QhullFacet facet);
	int getCalibrationPointIndex(vector<double> coordinates); 
	vector<double> getExtrapolated(vector<double> Point);
	bool simplexContainsAtLeastOneOfVertices(Simplex simplex, vector<QhullVertex> vertices);

	int isThereAnotherCalibrationPointAtPosition(vector<double> position);
	
public:
	void addCalibrationPoint(CalibrationPoint cp);
	vector<double> getInterpolated(vector<double> Point);

	int getNumberOfCalibrationPoints() {return (int)calibrationPoints.size();};

	void saveConfiguration(string Filename);
	void loadConfiguration(string Filename);
	string getConfiguration();
	void setConfiguration(string configuration);

	void tryToPerformTriangulation();

	QhullCalibrator(int SpaceDimension, int DataDimension);
	~QhullCalibrator(void);
};