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

#include "Simplex.h"
#include "QhullCalibrator.h"

Simplex::Simplex()
{

}

Simplex::~Simplex()
{
	cleanUp();
}

int Simplex::hasVertex(vector<double> vertex)
{
	int index = -1;

	for (int i=0; i<simplexPoints.size(); i++)
	{
		vector<double> coords = simplexPoints[i].getPoint();

		if (isEqual(coords, vertex))
			return i;
	}

	return index;
}

vector<int> Simplex::getConvexHullFacetsFacingPoint(vector<double> point)
{
	vector<int> indices;

	for (int i=0; i<cpFacetsOnConvexHull.size(); i++)
	{
		int vertexIndex = cpFacetsOnConvexHull[i][0];
		vector<double> pointOnFacet = simplexPoints[vertexIndex].getPoint();
		
		vector<double> vectorToPoint;
		for (int ii=0; ii<pointOnFacet.size(); ii++)
		{
			vectorToPoint.push_back(point[ii]-pointOnFacet[ii]);
		}

		double dotProduct = 0;
		for (int ii=0; ii<vectorToPoint.size(); ii++)
		{
			dotProduct+=vectorToPoint[ii]*facetNormals[i][ii];
		}

		if (dotProduct>0)
			indices.push_back(i);
	}

	return indices;
}

vector<double> Simplex::getProjectedPointOnFacet(vector<double> point, int facetIndex)
{
	vector<double> projectedPoint;

	//find the simplex point on the other side of the facet
	vector<double> oppositePoint;

	for (int i=0; i<simplexPoints.size(); i++)
	{
		bool vertexOnConvexFacet = false;

		for (int ii=0; ii<cpFacetsOnConvexHull[facetIndex].size(); ii++)
		{
			if ( isEqual( simplexPoints[cpFacetsOnConvexHull[facetIndex][ii]].getPoint(), simplexPoints[i].getPoint() )  )
			{
				vertexOnConvexFacet = true;
			}
		}

		if (!vertexOnConvexFacet)
		{
			oppositePoint = simplexPoints[i].getPoint();
		}
	}

	//get a vertex from the facet
	vector<double> aFacetVertex = simplexPoints[cpFacetsOnConvexHull[facetIndex][0]].getPoint();

	//get the facet normal and normilize it
	vector<double> normal = facetNormals[facetIndex];
	double length = 0;
	for (int i=0; i<normal.size(); i++)
		length += normal[i]*normal[i]; 

	length = sqrt(length);

	for (int i=0; i<normal.size(); i++)
		normal[i] = normal[i]/length; 
	

	//create the necessary vectors;
	vector<double> fromOppositeToFacetVertex = getVector(oppositePoint, aFacetVertex);
	vector<double> fromOppositeToPoint = getVector(oppositePoint, point);
	double scalingFactor = dotProduct(fromOppositeToFacetVertex, normal) / dotProduct(fromOppositeToPoint, normal);

	projectedPoint = addVectors(oppositePoint, multWithScalar(scalingFactor, fromOppositeToPoint));

	return projectedPoint;
}

double Simplex::dotProduct(vector<double> vector1, vector<double> vector2)
{
	double result = 0;

	for (int i=0; i<vector1.size(); i++)
	{
		result += vector1[i]*vector2[i];
	}

	return result;
}

vector<double> Simplex::getVector(vector<double> from, vector<double> to)
{
	vector<double> result;

	for (int i=0; i<from.size(); i++)
	{
		result.push_back(to[i]-from[i]);
	}

	return result;
}

vector<double> Simplex::addVectors(vector<double> vector1, vector<double> vector2)
{
	vector<double> result;

	for (int i=0; i<vector1.size(); i++)
	{
		result.push_back(vector1[i]+vector2[i]);
	}

	return result;
}

vector<double> Simplex::multWithScalar(double scalar, vector<double> vector1)
{
	vector<double> result;

	for (int i=0; i<vector1.size(); i++)
	{
		result.push_back(vector1[i]*scalar);
	}

	return result;
}

vector<vector<double>> Simplex::getFacetPoints(int facetIndex)
{
	vector<vector<double>> result;

	vector<int> vertexIndices = cpFacetsOnConvexHull[facetIndex];

	for (int i=0; i<vertexIndices.size(); i++)
	{
		vector<double> vertex = simplexPoints[vertexIndices[i]].getPoint();

		result.push_back(vertex);
	}

	return result;
}

double Simplex::getWeightingFactor(vector<double> point, int facetIndex) 
{
	vector<double> origin = simplexPoints[cpFacetsOnConvexHull[facetIndex][0]].getPoint();
	vector<double> lowerDimensionPoint = mult(matrix[facetIndex], getVector(origin, point));
	lowerDimensionPoint.erase(lowerDimensionPoint.begin()+lowerDimensionPoint.size()-1);

	double result = calibrators[facetIndex]->getInterpolated(lowerDimensionPoint)[0];

	if (result<0)
		result = 0;
	else if (result==0)
		result = -1; //on ridge (special case)

	return result;
}

vector<double> Simplex::getInterpolationResult(vector<double> Point)
{
	int spaceDimension = Point.size();
	int dataDimension = simplexPoints[0].getData().size();

	double **t = new double*[spaceDimension];
	double **it = new double*[spaceDimension];
	for (int i=0; i<spaceDimension; i++)
	{
		t[i] = new double[spaceDimension*2];
		it[i] = new double[spaceDimension*2];
	}

	for (int i=0; i<spaceDimension; i++)
		for (int j=0; j<spaceDimension; j++)
		{
			t[i][j]=simplexPoints[j].getPoint()[i]-simplexPoints.back().getPoint()[i];
		}

	int inversionresult=inverse(t, it, spaceDimension);

	if (inversionresult == 0)
		cannotInvertMatrix = true;
	else
		cannotInvertMatrix = false;

	vector<double> r;
	for (int i=0; i<spaceDimension; i++)
		r.push_back(Point[i]-simplexPoints.back().getPoint()[i]);

	vector<double> l; 

	for (int i=0; i<spaceDimension; i++)  //l=it*r;
	{
		double sum=0;
		for (int j=0; j<spaceDimension; j++)
		{
			sum+=it[i][j]*r[j];
		}
		l.push_back(sum);
	}

	vector<double> factors;
	double sum=0;
	for (int i=0; i<spaceDimension; i++)
	{
		factors.push_back(l[i]);
		sum+=l[i];
	}
	factors.push_back(1-sum); 
	
	//Interpolation
	vector<double> result;

	for (int i=0; i<dataDimension; i++)
	{
		double temp=0;
		for (int j=0; j<spaceDimension+1; j++)
		{
			temp+=factors[j]*simplexPoints[j].getData()[i];
		}
		result.push_back(temp);
	}  

	for (int i=0; i<spaceDimension; i++)
	{
		delete t[i]; 
		delete it[i]; 
	}
	
	delete t; 
	delete it;

	return result;
}

int Simplex::inverse(double **a, double **ainv, int n) 
{
	int   i, j;                    // Zeile, Spalte
	int   s;                       // Elimininationsschritt
	int   pzeile;                  // Pivotzeile
	int   fehler = 0;              // Fehlerflag
	double f;                      // Multiplikationsfaktor
	const double Epsilon = 0.01;   // Genauigkeit
	double Maximum;                // Zeilenpivotisierung
	extern FILE *fout;
	int pivot = 1;

	// ergänze die Matrix a um eine Einheitsmatrix (rechts anhängen)
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
		{
			a[i][n+j] = 0.0;
			if (i == j)
				a[i][n+j] = 1.0;
		}
	}

	// die einzelnen Eliminationsschritte
	s = 0;
	do {
		// Pivotisierung vermeidet unnötigen Abbruch bei einer Null in der Diagnonalen und
		// erhöht die Rechengenauigkeit
		Maximum = fabs(a[s][s]);
		if (pivot)
		{
			pzeile = s ; 
			for (i = s+1; i < n; i++)
				if (fabs(a[i][s]) > Maximum) {
					Maximum = fabs(a[i][s]) ;
					pzeile = i;
				}
		}
		fehler = (Maximum < Epsilon);

		if (fehler) break;           // nicht lösbar 

		if (pivot)
		{
			if (pzeile != s)  // falls erforderlich, Zeilen tauschen
			{ double h;
			for (j = s ; j < 2*n; j++) {
				h = a[s][j];
				a[s][j] = a[pzeile][j];
				a[pzeile][j]= h;
			}
			}
		}

		// Eliminationszeile durch Pivot-Koeffizienten f = a[s][s] dividieren
		f = a[s][s];
		for (j = s; j < 2*n; j++)
			a[s][j] = a[s][j] / f;

		// Elimination --> Nullen in Spalte s oberhalb und unterhalb der Diagonalen
		// durch Addition der mit f multiplizierten Zeile s zur jeweiligen Zeile i
		for (i = 0; i < n; i++ ) {
			if (i != s) 
			{
				f = -a[i][s];                 // Multiplikationsfaktor
				for (j = s; j < 2*n ; j++)    // die einzelnen Spalten
					a[i][j] += f*a[s][j];       // Addition der Zeilen i, s
			}
		}

		s++;
	} while ( s < n ) ;

	if (fehler) 
	{
		printf ("Inverse: Matrix ist singulär\n");
		return 0; 
	}
	// Die angehängte Einheitsmatrix Matrix hat sich jetzt in die inverse Matrix umgewandelt
	// Umkopieren auf die Zielmatrix
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			ainv[i][j] = a[i][n+j];
		}
	} 
	return 1;  
}

bool Simplex::isEqual(vector<double> vector1, vector<double> vector2)
{
	for (int i=0; i<vector1.size(); i++)
	{
		if (vector1[i]!=vector2[i])
			return false;
	}

	return true;
}

void Simplex::initializeSimplex(vector<QhullFacet> ConvexFacets)
{
	for(int i=0; i<ConvexFacets.size(); i++)
	{
		vector<vector<double>> points;
		vector<QhullVertex> vertices = ConvexFacets[i].vertices().toStdVector();

		for (int j=0; j<vertices.size(); j++)
		{
			vector<double> point = vertices[j].point().toStdVector();
			points.push_back(point);
		}
	}

	cleanUp();

	int spaceDimension = simplexPoints[0].getPoint().size();

	for (int i=0; i<ConvexFacets.size(); i++)
	{
		vector<QhullVertex> vertices = ConvexFacets[i].vertices().toStdVector();

		bool simplexHasConvexFacet=true;
		vector<int> vertexIndices;
		vector<int> cpIndices;

		for (int ii=0; ii<vertices.size(); ii++)
		{
			vector<double> coordinates = vertices[ii].point().toStdVector();
			
			int vertexIndex = hasVertex(coordinates);

			if (vertexIndex==-1)
				simplexHasConvexFacet = false;
			else
			{
				cpIndices.push_back(vertexIndex);		
			}
		}
		
		if (simplexHasConvexFacet)
		{
			cpFacetsOnConvexHull.push_back(cpIndices);
			int facetIndex = cpFacetsOnConvexHull.size()-1;

			vector<double> normal;
			for (int j=0; j<spaceDimension; j++)
			{
				normal.push_back(ConvexFacets[i].getFacetT()->normal[j]);
			}
			facetNormals.push_back(normal);

			bool inverted = false;
			double **projectionMatrix = getProjectionMatrix(facetIndex, inverted);

			QhullCalibrator* calibrator = new QhullCalibrator(spaceDimension-1, 1);
			if (inverted)
			{
				vector<double> origin = simplexPoints[cpIndices[0]].getPoint();
				vector< vector<double> > facetPoints;
				for (int ii=0; ii<cpIndices.size(); ii++)
					facetPoints.push_back(getVector(origin, simplexPoints[cpIndices[ii]].getPoint()));

				for (int ii=0; ii<facetPoints.size(); ii++)
					facetPoints[ii] = mult(projectionMatrix, facetPoints[ii]);

				//remove last coordinate
				for (int ii=0; ii<facetPoints.size(); ii++)
					facetPoints[ii].erase(facetPoints[ii].begin()+facetPoints[ii].size()-1);

				vector<double> zeroValue;
				zeroValue.push_back(0);
				vector<double> oneValue;
				oneValue.push_back(1);

				vector<double> barycenter;
				for (int k=0; k<facetPoints.size(); k++)
				{
					for (int ii=0; ii<facetPoints[k].size(); ii++)
					{
						if (barycenter.size()<=ii)
							barycenter.push_back(0);

						barycenter[ii] = barycenter[ii]+facetPoints[k][ii];
					}

					CalibrationPoint cp(facetPoints[k], zeroValue);
					calibrator->addCalibrationPoint(cp);  //Add calibration points for each vertex (with 0 data value)
				}
				for (int k=0; k<barycenter.size(); k++)
					barycenter[k] = barycenter[k]/facetPoints.size();

				CalibrationPoint cpBarycenter(barycenter, oneValue);
				calibrator->addCalibrationPoint(cpBarycenter); //Add calibration point for the barycenter (with 1 data value)

				calibrator->tryToPerformTriangulation();
			}
			else
			{
				for (int k=0; k<spaceDimension; k++)
					delete [] projectionMatrix[k];

				delete [] projectionMatrix;
			}

			matrix.push_back(projectionMatrix);
			calibrators.push_back(calibrator);
		}
		
	}
}

int Simplex::getNumberOfDuplicates (int dimension, vector<vector<double>> pointCloud)
{
	int numberOfPoints = pointCloud.size();
	int numberOfDublicates=0;

	for (int i=0; i<numberOfPoints; i++)
		for (int j=0; j<numberOfPoints; j++)
			if (pointCloud[i][dimension] == pointCloud[j][dimension] && i!=j)
				numberOfDublicates++;


	return numberOfDublicates;
}

double Simplex::getVariance(int dimension, vector<vector<double>> pointCloud)
{
	double mean = 0;
	int numberOfPoints = pointCloud.size();

	for (int i=0; i<numberOfPoints; i++)
		mean+=pointCloud[i][dimension];

	mean = mean/numberOfPoints;

	double variance = 0;
	for (int i=0; i<numberOfPoints; i++)
		variance += (mean-pointCloud[i][dimension])*(mean-pointCloud[i][dimension]);

	return variance;
}

int Simplex::getDimensionToOmmit(vector<vector<double>> pointCloud)
{
	int numberOfPoints = pointCloud.size();
	int numberOfDimensions = pointCloud[0].size();

	vector<double> variances; 
	vector<double> duplicates;

	for (int i=0; i<numberOfDimensions; i++)
	{
		variances.push_back(getVariance(i, pointCloud));
		duplicates.push_back(getNumberOfDuplicates(i, pointCloud));
	}

	vector<int> sortIndices;
	for (int i=0; i<numberOfDimensions; i++)
		sortIndices.push_back(i);

	//bubble sort
	for (int i = 0; i<numberOfDimensions-1; ++i) 
    {
		for (int j = 0; j<numberOfDimensions-i-1; ++j) 
        {
			if (variances[sortIndices[j]] > variances[sortIndices[j + 1]]) 
            {
                int tmp = sortIndices[j];
                sortIndices[j] = sortIndices[j + 1];
                sortIndices[j + 1] = tmp;
            }
        }
	}

	for (int i=0; i<numberOfDimensions; i++)
	{
		if (duplicates[sortIndices[i]] == 0)
			return i;
	}

	return -1;
}

vector<double> Simplex::mult(double **a, vector<double> b)
{
	vector<double> result;
	int dimension = b.size();

	for (int i=0; i<dimension; i++)
	{
		double sum = 0;

		for (int ii=0; ii<dimension; ii++)
		{
			sum+= b[ii]*a[i][ii];
		}

		result.push_back(sum);
	}

	return result;
}

double** Simplex::getProjectionMatrix(int facetIndex, bool& inverted)
{
	vector<vector<double>> facetPoints;

	for (int i=0; i<cpFacetsOnConvexHull[facetIndex].size(); i++)
		facetPoints.push_back(simplexPoints[cpFacetsOnConvexHull[facetIndex][i]].getPoint());

	vector<double> facetNormal = facetNormals[facetIndex];

	//prepare the output matrix
	int spaceDimension = simplexPoints[0].getPoint().size();
	double **ia = new double*[spaceDimension];
	double **a = new double*[spaceDimension];
	for (int i=0; i<spaceDimension; i++)
	{
		a[i] = new double[spaceDimension*2];
		ia[i] = new double[spaceDimension*2];
	}

	//fill the matrix A
	for (int i=0; i<spaceDimension; i++)
		for (int j=0; j<spaceDimension; j++)
		{
			if (j == spaceDimension-1)
			{
				a[i][j] = facetNormal[i];
			}
			else
			{
				a[i][j] = facetPoints[j+1][i] - facetPoints[0][i];
			}
		}


	int inversionResult = inverse(a, ia, spaceDimension);
	if (inversionResult)
		inverted = true;
	else
		inverted = false;

	//clean up
	for (int i=0; i<spaceDimension; i++)
		delete [] a[i];
	delete [] a;
	
	return ia;
}

void Simplex::cleanUp()
{
	cpFacetsOnConvexHull.clear();
	facetNormals.clear();

	for (int i=0; i<calibrators.size(); i++)
		delete calibrators[i];

	calibrators.clear();

	int spaceDimension = simplexPoints[0].getPoint().size();
	for (int i=0; i<matrix.size(); i++)
	{
		for (int j=0; j<spaceDimension; j++)
		{
			if (matrix[i][j])
				delete [] matrix[i][j];
		}

		delete [] matrix[i];
	}
	matrix.clear();
}
