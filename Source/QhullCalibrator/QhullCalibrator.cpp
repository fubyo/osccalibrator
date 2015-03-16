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

#include "QhullCalibrator.h"

void QhullCalibrator::addCalibrationPoint(CalibrationPoint cp)
{
	vector<double> cpPoint=cp.getPoint();
	vector<double> cpData=cp.getData();

	int cpIndex = isThereAnotherCalibrationPointAtPosition(cpPoint);

	if (cpIndex==-1)
	{
		vector<double> point;
		for (unsigned int i=0; i<spaceDimension; i++)
		{
			if (i<cpPoint.size())
			{
				point.push_back(cpPoint[i]);
			}
			else
			{
				point.push_back(0);
			}
		}

		vector<double> data;
		for (unsigned int i=0; i<dataDimension; i++)
		{
			if (i<cpData.size())
				data.push_back(cpData[i]);
			else
				data.push_back(0);
		}

		CalibrationPoint CP(point, data);
		calibrationPoints.push_back(CP);
	}
	else
		calibrationPoints[cpIndex].setData(cpData);
}

double QhullCalibrator::distance(CalibrationPoint A, CalibrationPoint B)
{
	double sum=0;
	vector<double> pointA=A.getPoint();
	vector<double> pointB=B.getPoint();

	for (int i=0; i<spaceDimension; i++)
	{
		sum+=(pointA[i]-pointB[i])*(pointA[i]-pointB[i]);
	}

	return sqrt(sum);
}

vector<CalibrationPoint> QhullCalibrator::getSimplexPoints(vector<double> Point)
{
	vector<CalibrationPoint> result;
	
	double* point = new double[spaceDimension+1];
	point[spaceDimension]=0;
	for (int i=0; i<spaceDimension; i++)
	{
		point[i] = Point[i];
		point[spaceDimension] += point[i]*point[i];
	}

	double bestdist[1];
	unsigned int isoutside[1];

	qh_restore_qhull(&qHullContext);
	QhullFacet facet(qh_findbestfacet(point, !qh_ALL, bestdist, isoutside));
	qHullContext = qh_save_qhull();

	delete [] point;

	int simplexIndex = -1;
	for (int i=0; i<simplices.size(); i++)
	{
		if (simplices[i]->id == facet.id())
			simplexIndex = i;
	}

	if (simplexIndex!=-1)
		result = simplices[simplexIndex]->simplexPoints;

	return result;
}

vector<double> QhullCalibrator::getInterpolated(vector<double> Point)
{
	if (calibrationPoints.size()==1) //If only one calibration point available, just return its dataset.
	{
		lastInterpolationResult = calibrationPoints[0].getData();
		return lastInterpolationResult;
	}
	else if (spaceDimension==1 && calibrationPoints.size()>1) //For single dimensional calibration points, interpolate accordingly  
	{		
		//sort(calibrationPoints.begin(), calibrationPoints.end(), &Calibrator::calibrationPointSortPredicate);

		vector<double> result;

		if (Point[0]<(calibrationPoints[0].getPoint())[0])
		{
			double distanceCC = sqrt((calibrationPoints[0].getPoint()[0]-calibrationPoints[1].getPoint()[0]) //Distance between the first two calibration points
									*(calibrationPoints[0].getPoint()[0]-calibrationPoints[1].getPoint()[0]));

			double distancePC = sqrt((calibrationPoints[0].getPoint()[0]-Point[0]) //Distance between the Point and the first calibration point
									*(calibrationPoints[0].getPoint()[0]-Point[0]));

			for (int i=0; i<dataDimension; i++)
			{
				result.push_back(calibrationPoints[0].getData()[i] + distancePC * (calibrationPoints[0].getData()[i] - calibrationPoints[1].getData()[i])/distanceCC);
			}
		}
		else if (Point[0]>(calibrationPoints.end()-1)->getPoint()[0])
		{
			double distanceCC = sqrt(((calibrationPoints.end()-1)->getPoint()[0]-(calibrationPoints.end()-2)->getPoint()[0]) //Distance between the last two calibration points
									*((calibrationPoints.end()-1)->getPoint()[0]-(calibrationPoints.end()-2)->getPoint()[0]));

			double distancePC = sqrt(((calibrationPoints.end()-1)->getPoint()[0]-Point[0]) //Distance between the Point and the last calibration point
									*((calibrationPoints.end()-1)->getPoint()[0]-Point[0]));

			for (int i=0; i<dataDimension; i++)
			{
				result.push_back((calibrationPoints.end()-1)->getData()[i] + distancePC * ((calibrationPoints.end()-1)->getData()[i] - (calibrationPoints.end()-2)->getData()[i])/distanceCC);
			}
		}
		else
		{
			int i=0;
			do 
			{
				i++;
			}while (Point[0]>calibrationPoints[i].getPoint()[0]);

			//Point between calibrationPoint[i-1] and calibrationPoint[i]

			double distanceCC = sqrt((calibrationPoints[i-1].getPoint()[0]-calibrationPoints[i].getPoint()[0]) //Distance between the two calibration points
									*(calibrationPoints[i-1].getPoint()[0]-calibrationPoints[i].getPoint()[0]));

			double distancePC = sqrt((calibrationPoints[i-1].getPoint()[0]-Point[0]) //Distance between the Point and the first calibration point
									*(calibrationPoints[i-1].getPoint()[0]-Point[0]));

			for (int ii=0; ii<dataDimension; ii++)
			{
				result.push_back(calibrationPoints[i-1].getData()[ii] + distancePC * (calibrationPoints[i].getData()[ii] - calibrationPoints[i-1].getData()[ii])/distanceCC);
			}
			
		}

		lastInterpolationResult = result;
		return lastInterpolationResult;
	}
	else if (spaceDimension>1 && calibrationPoints.size()==2) //With only two interpolation points, difining a line, we have to project our point on the line in order to interpolate.
	{
		vector<double> result;

		vector<double> lineVector;
		for (int i=0; i<spaceDimension; i++)
			lineVector.push_back(calibrationPoints[1].getPoint()[i] - calibrationPoints[0].getPoint()[i]);

		double squareSum = 0;
		for (int i=0; i<spaceDimension; i++)
			squareSum+=lineVector[i]*lineVector[i];

		double length = sqrt(squareSum);

		//Normalize
		for (int i=0; i<spaceDimension; i++)
			lineVector[i]=lineVector[i]/length;

		vector<double> pointVector;
		for (int i=0; i<spaceDimension; i++)
			pointVector.push_back(Point[i] - calibrationPoints[0].getPoint()[i]);


		//Interpolation factor t
		double t=0;
		for (int i=0; i<spaceDimension; i++)
			t+=pointVector[i]*lineVector[i];

		double distanceCC = distance(calibrationPoints[0], calibrationPoints[1]);

		for (int ii=0; ii<dataDimension; ii++)
		{
			result.push_back( calibrationPoints[0].getData()[ii] + t*(calibrationPoints[1].getData()[ii]-calibrationPoints[0].getData()[ii])/distanceCC );
		}

		lastInterpolationResult = result;
		return lastInterpolationResult;
	}
	else if (spaceDimension==3 && calibrationPoints.size()==3) //With only three calibration points, defining a plane, project on the plane to interpolate
	{
		vector<double> result;
		vector<double> A = calibrationPoints[0].getPoint();
		vector<double> B = calibrationPoints[1].getPoint();
		vector<double> C = calibrationPoints[2].getPoint();

		///////

		vector<double> u;
		for (int i=0; i<spaceDimension; i++)
			u.push_back(B[i] - A[i]);

		vector<double> v;
		for (int i=0; i<spaceDimension; i++)
			v.push_back(C[i] - A[i]);

		////////

		vector<double> n = crossProduct(u,v);

		double oneOver4ASquared = 1.0 / dotProduct(n, n);

		vector<double> w;
		for (int i=0; i<spaceDimension; i++)
			w.push_back(Point[i] - A[i]);

		////////

		double b2 = dotProduct(crossProduct(u, w), n) * oneOver4ASquared;
		double b1 = dotProduct(crossProduct(w, v), n) * oneOver4ASquared;
		double b0 = 1.0 - b1 - b2;

		///////

		for (int i=0; i<dataDimension; i++)
		{
			result.push_back(b0*calibrationPoints[0].getData()[i]+
							 b1*calibrationPoints[1].getData()[i]+
							 b2*calibrationPoints[2].getData()[i]);
		}

		lastInterpolationResult = result;
		return lastInterpolationResult;
	}
	else if (calibrationPoints.size()>(unsigned int)spaceDimension) //If there are enough calibration points, perform a delaunay triangulation and get the corresponding simplex in order to interpolate
	{
		
		vector<CalibrationPoint> simplexPoints;

		if (calibrationPoints.size()>(unsigned int)spaceDimension+1)
		{
			//Check if Point is inside the convex hull.
			double* point = new double[spaceDimension+1];
			point[spaceDimension]=0;
			for (int i=0; i<spaceDimension; i++)
			{
				point[i] = Point[i];
				point[spaceDimension] += point[i]*point[i];
			}

			double bestdist[1];
			unsigned int isoutside[1];
			isoutside[0] = 0;

			qh_restore_qhull(&qHullConvexContext);
			qh_findbestfacet(point, !qh_ALL, bestdist, isoutside);
			qHullConvexContext=qh_save_qhull();
			delete [] point;

			if (isoutside[0])
			{
				return getExtrapolated(Point);
			}
			else
			{
				simplexPoints=getSimplexPoints(Point);
			}
		}
		else
		{
			simplexPoints=calibrationPoints;
		}

		if (simplexPoints.size()>(unsigned int)spaceDimension) //The simplex is found
		{
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
				delete [] t[i]; 
				delete [] it[i]; 
			}
			delete [] t; 
			delete [] it;

			lastInterpolationResult = result;
		}
		
		return lastInterpolationResult;
	}
	else //just return zeros
	{
		vector<double> result;

		for (int i=0; i<dataDimension; i++)
		{
			result.push_back(0);
		} 
		return result;
	}
}

QhullCalibrator::QhullCalibrator(int SpaceDimension, int DataDimension)
{
	spaceDimension=SpaceDimension;
	dataDimension=DataDimension;

	points = 0;

	qHullContext=0;
	qHullConvexContext=0;
}

QhullCalibrator::~QhullCalibrator(void)
{
	for (int i=0; i<simplices.size(); i++)
		delete simplices[i];

	if (points)
	{
		delete [] points;
		points = 0;
	}

	if (qHullContext)
	{
		qh_restore_qhull(&qHullContext);
		qh_freeqhull(qh_ALL);
	}

	if (qHullConvexContext)
	{
		qh_restore_qhull(&qHullConvexContext);
		qh_freeqhull(qh_ALL);
	}
}

void QhullCalibrator::saveConfiguration(string Filename)
{
	ofstream configfile;
	
	configfile.open(Filename);
	
	configfile << spaceDimension << '\n'
			   << dataDimension << '\n'
			   << calibrationPoints.size() << '\n' ;

	for (unsigned int i=0; i<calibrationPoints.size(); i++)
	{
		configfile << '\n';

		vector<double> point=calibrationPoints[i].getPoint();
		for (int j=0; j<spaceDimension; j++)
		{
			configfile << point[j] << ' ';
		}

		configfile << '\n';

		vector<double> data=calibrationPoints[i].getData();
		for (int j=0; j<dataDimension; j++)
		{
			configfile << data[j] << ' ';
		}
	}

	configfile.close();
}

void QhullCalibrator::loadConfiguration(string Filename)
{
	string line;
	int numberOfCalibrationPoints;
	ifstream configfile(Filename);

	if (configfile.is_open())
	{
		getline(configfile,line);
		spaceDimension = atoi(line.c_str());

		getline(configfile,line);
		dataDimension = atoi(line.c_str());
	
		getline(configfile,line);
		numberOfCalibrationPoints = atoi(line.c_str());

		getline(configfile,line);

		calibrationPoints.clear();

		for (int i=0; i<numberOfCalibrationPoints; i++)
		{
			vector<double> point;
			for (int j=0; j<spaceDimension; j++)
			{
				double temp;
				configfile >> temp;
				point.push_back(temp);
			}

			vector<double> data;
			for (int j=0; j<dataDimension; j++)
			{
				double temp;
				configfile >> temp;
				data.push_back(temp);
			}

			CalibrationPoint cp;
			cp.setPoint(point);
			cp.setData(data);

			calibrationPoints.push_back(cp);

			tryToPerformTriangulation();
		}

		configfile.close();
	}
}

int QhullCalibrator::inverse(double **a, double **ainv, int n) 
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

bool QhullCalibrator::calibrationPointSortPredicate(const CalibrationPoint& a, const CalibrationPoint& b)
{
	return a.getPoint()[0] < b.getPoint()[0];
}

//3d routines
vector<double> QhullCalibrator::crossProduct(vector<double> a, vector<double> b)
{
	vector<double> result;
	result.push_back(a[1]*b[2]-a[2]*b[1]);
	result.push_back(a[2]*b[0]-a[0]*b[2]);
	result.push_back(a[0]*b[1]-a[1]*b[0]);
	return result;
}

double QhullCalibrator::dotProduct(vector<double> a, vector<double> b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

string QhullCalibrator::getConfiguration()
{
	stringstream  configuration;
	
	configuration << spaceDimension << '\n'
			   << dataDimension << '\n'
			   << calibrationPoints.size() << '\n' ;

	for (unsigned int i=0; i<calibrationPoints.size(); i++)
	{
		configuration << '\n';

		vector<double> point=calibrationPoints[i].getPoint();
		for (int j=0; j<spaceDimension; j++)
		{
			configuration << point[j] << ' ';
		}

		configuration << '\n';

		vector<double> data=calibrationPoints[i].getData();
		for (int j=0; j<dataDimension; j++)
		{
			configuration << data[j] << ' ';
		}
	}

	return configuration.str();
}

void QhullCalibrator::setConfiguration(string configuration)
{
	stringstream configfile(configuration);

	string line;
	int numberOfCalibrationPoints;
	{
		getline(configfile,line);
		spaceDimension = atoi(line.c_str());

		getline(configfile,line);
		dataDimension = atoi(line.c_str());
	
		getline(configfile,line);
		numberOfCalibrationPoints = atoi(line.c_str());

		getline(configfile,line);

		calibrationPoints.clear();

		for (int i=0; i<numberOfCalibrationPoints; i++)
		{
			vector<double> point;
			for (int j=0; j<spaceDimension; j++)
			{
				double temp;
				configfile >> temp;
				point.push_back(temp);
			}

			vector<double> data;
			for (int j=0; j<dataDimension; j++)
			{
				double temp;
				configfile >> temp;
				data.push_back(temp);
			}

			CalibrationPoint cp;
			cp.setPoint(point);
			cp.setData(data);

			calibrationPoints.push_back(cp);
		}

		tryToPerformTriangulation();
	}
}

void QhullCalibrator::performTriangulation()
{
	int pointDimension = spaceDimension; 
	int pointCount = (int)calibrationPoints.size();

	for (int i=0; i<simplices.size(); i++)
		delete simplices[i];

	simplices.clear();

	//clear the points
	if (points != 0)
	{
		delete [] points;
		points = 0;
	}
	points = new double[spaceDimension*pointCount];
	
	//fill the points with values
	int index = 0;
	for (int i=0; i<calibrationPoints.size(); i++)
	{
		for (int ii=0; ii<calibrationPoints[i].getPoint().size(); ii++)
		{
			points[index] = calibrationPoints[i].getPoint()[ii];
			index++;
		}
	}

	if (pointCount>=spaceDimension+2)
	{
		if (qHullContext)
		{
			qh_restore_qhull(&qHullContext);
			qh_freeqhull(qh_ALL);
		}

		if (qHullConvexContext)
		{
			qh_restore_qhull(&qHullConvexContext);
			qh_freeqhull(qh_ALL);
		}

		if (spaceDimension == 2 || spaceDimension == 3)
			qh_new_qhull(pointDimension, pointCount, points, 0, "qhull d Qt Qz", 0, &errFile);
		else
			qh_new_qhull(pointDimension, pointCount, points, 0, "qhull d Qt Qz", 0, &errFile);
		facetT *facet;
		vector<QhullFacet> facets;
		FORALLfacets
		{
			facets.push_back(QhullFacet(facet));
		}
		qHullContext = qh_save_qhull();

		qh_new_qhull(pointDimension, pointCount, points, 0, "qhull ", 0, &errFile);
		vector<QhullFacet> convexFacets;
		FORALLfacets
		{
			convexFacets.push_back(QhullFacet(facet));
		}
		qHullConvexContext = qh_save_qhull();
	
		for (int i=0; i<facets.size(); i++)
		{
			bool isSimplicial = facets[i].isSimplicial();
			
			if (isSimplicial)
			{
				Simplex* simplex = new Simplex();
				simplex->id = facets[i].id();

				vector<int> calibrationPointIndices = getIndicesOfCalibrationPoints(facets[i]);
				for (int ii=0; ii<calibrationPointIndices.size(); ii++)
				{
					simplex->simplexPoints.push_back(calibrationPoints[calibrationPointIndices[ii]]);
				}
				
				simplex->initializeSimplex(convexFacets);
				
				if (simplex->simplexPoints.size() == spaceDimension+1)
					simplices.push_back(simplex);
				else
					delete simplex;
			}
		}

		
	}

	//after configuring the simplices, we clean the points up.	
	if (points != 0)
	{
		delete [] points;
		points = 0;
	}
}

vector<int> QhullCalibrator::getIndicesOfCalibrationPoints(QhullFacet facet)
{
	vector<int> result;

	vector<QhullVertex> vertices = facet.vertices().toStdVector();
	for (int i=0; i<vertices.size(); i++)
	{
		for (int ii = 0; ii<calibrationPoints.size(); ii++)
		{
			bool calibrationPointFound = true;

			for (int iii=0; iii<calibrationPoints[ii].getPoint().size(); iii++)
			{
				if (calibrationPoints[ii].getPoint()[iii] != vertices[i].point().toStdVector()[iii])
					calibrationPointFound = false;
			}

			if (calibrationPointFound)
				result.push_back(ii);
		}
	}

	return result;
}

int QhullCalibrator::getCalibrationPointIndex(vector<double> coordinates)
{
	bool found = false;
	int index = -1;

	for (int i=0; i<calibrationPoints.size(); i++)
	{
		vector<double> calibrationPointCoords = calibrationPoints[i].getPoint();

		bool vertexFound = true;
		for (int ii=0; ii<coordinates.size(); ii++)
		{
			if (coordinates[ii] != calibrationPointCoords[ii])
				vertexFound = false;
		}

		if (vertexFound)
		{
			found = true;
			index = i;
			return index;
		}
	}

	return index;
}

vector<double> QhullCalibrator::getExtrapolated(vector<double> Point)
{
	vector<ExtrapolationResult> extrapolationResults;

	//Iterate over the simpleces and find the ones that may take part in the extrapolation
	for (int i=0; i<simplices.size(); i++)
	{
		if (simplices[i]->cpFacetsOnConvexHull.size()) //if it has faces on the convex hull
		{
			vector<int> facetIndices = simplices[i]->getConvexHullFacetsFacingPoint(Point); //find the facets of the simplex on the convex hull that are facing the point.

			if (facetIndices.size()>1)
			{
				vector<double> result = simplices[i]->getInterpolationResult(Point); //this simplex is sufficient to extrapolate from.

				if (!simplices[i]->cannotInvertMatrix)
					lastInterpolationResult = result;
					
				simplices[i]->cannotInvertMatrix = false;
				return  lastInterpolationResult;
			}
			else if (facetIndices.size()==1)
			{
				vector<double> projectedPoint = simplices[i]->getProjectedPointOnFacet(Point, facetIndices[0]); //project the point on the facet
				
				ExtrapolationResult er;
				er.weight = simplices[i]->getWeightingFactor(projectedPoint, facetIndices[0]);
				er.values = simplices[i]->getInterpolationResult(Point);

				if (er.weight==-1) //special case : projected point along rigde
				{
					er.weight=0;
					er.onRidge=true;
				}
				else
					er.onRidge=false;

				extrapolationResults.push_back(er);	
			}
		}
	}

	//after gathering all the extrapolation results, together with the weights, we calculate the end result.
	vector<double> result;
	double weightSum = 0;
	for (int j=0; j<extrapolationResults.size(); j++)
			weightSum+=extrapolationResults[j].weight;

	//if weightSum is 0, then look for the first result with onRidge and return the values... else return the last result.
	if (weightSum==0)
	{
		if (extrapolationResults.size() == 1)
			return extrapolationResults[0].values;

		for (int i=0; i<extrapolationResults.size(); i++)
		{
			if (extrapolationResults[i].onRidge)
				return extrapolationResults[i].values;
		}

		return lastInterpolationResult;
	}


	for (int i=0; i<dataDimension; i++)
	{
		double value=0;

		for (int j=0; j<extrapolationResults.size(); j++)
		{
			value+=extrapolationResults[j].values[i]*extrapolationResults[j].weight;
		}

		value = value/weightSum;
		result.push_back(value);
	}

	return result;
}

bool QhullCalibrator::simplexContainsAtLeastOneOfVertices(Simplex simplex, vector<QhullVertex> vertices)
{
	for (int i=0; i<simplex.simplexPoints.size(); i++)
	{
		vector<double> simplexVertexCoordinates = simplex.simplexPoints[i].getPoint();

		for (int ii=0; ii<vertices.size(); ii++)
		{
			vector<double> vertexCoordinates = vertices[i].point().toStdVector();

			bool coordinatesAreTheSame = true;
			for (int iii=0; iii<vertexCoordinates.size(); iii++)
			{
				if (vertexCoordinates[iii] != simplexVertexCoordinates[iii])
					coordinatesAreTheSame = false;
			}

			if (coordinatesAreTheSame)
				return true;
		}
	}

	return false;
}

void QhullCalibrator::tryToPerformTriangulation()
{
	if (spaceDimension==1)
		sort(calibrationPoints.begin(), calibrationPoints.end(), &QhullCalibrator::calibrationPointSortPredicate);

	if (spaceDimension>=2)
		performTriangulation(); 
}

int QhullCalibrator::isThereAnotherCalibrationPointAtPosition(vector<double> position)
{
	for (int i=0; i<calibrationPoints.size(); i++)
	{
		vector<double> cpPosition = calibrationPoints[i].getPoint();

		bool coordinatesAreTheSame = true;

		for (int j=0; j<position.size(); j++)
			if (position[j]!=cpPosition[j])
				coordinatesAreTheSame = false;

		if (coordinatesAreTheSame)
			return i;
	}

	return -1;
}
