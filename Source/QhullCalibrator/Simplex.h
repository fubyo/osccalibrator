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
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <limits>
#include <set>
using namespace std;

#include "../CalibrationPoint.h"

#include "../../qhull/src/libqhullcpp/QhullFacetList.h"
#include "../../qhull/src/libqhullcpp/QhullFacetSet.h"
#include "../../qhull/src/libqhullcpp/QhullVertexSet.h"
#include "../../qhull/src/libqhullcpp/Qhull.h"
#include "../../qhull/src/libqhullcpp/UsingLibQhull.h"

using namespace orgQhull;

class QhullCalibrator;

class Simplex
{
public:
	int id;
	vector<CalibrationPoint> simplexPoints;

	vector< vector<int> > cpFacetsOnConvexHull; //the calibration point indices for each facet on the convex hull.
	vector< vector<double> > facetNormals; //the normals of the facets
	vector<int> dimensionToOmmit; //the dimension to ommit after projecting a point on the facet. (used for the extrapolation)
	vector<double**> matrix;
	vector<QhullCalibrator*> calibrators;

	int hasVertex(vector<double> vertex);

	vector<int> getConvexHullFacetsFacingPoint(vector<double> point);
	vector<double> getProjectedPointOnFacet(vector<double> point, int facetIndex);
	double dotProduct(vector<double> vector1, vector<double> vector2);
	vector<double> getVector(vector<double> from, vector<double> to);
	vector<double> addVectors(vector<double> vector1, vector<double> vector2);
	vector<double> multWithScalar(double scalar, vector<double> vector1);
	vector<vector<double>> getFacetPoints(int facetIndex);

	bool isEqual(vector<double> vector1, vector<double> vector2);

	double getWeightingFactor(vector<double> point, int facetIndex);

	int inverse(double **a, double **ainv, int n);
	vector<double> mult(double **a, vector<double> b);
	double** getProjectionMatrix(int facetIndex, bool& inverted);

	vector<double> getInterpolationResult(vector<double> Point);
	void initializeSimplex(vector<QhullFacet> ConvexFacets);

	int getNumberOfDuplicates (int dimension, vector<vector<double>> pointCloud);
	double getVariance(int dimension, vector<vector<double>> pointCloud);
	int getDimensionToOmmit(vector<vector<double>> pointCloud);

	bool cannotInvertMatrix;

	void cleanUp();
		
	Simplex();
	~Simplex();
};