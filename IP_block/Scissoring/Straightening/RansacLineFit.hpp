#ifndef QORQL_SCISSORING_STRAIGHTENING_RANSACLINEFIT_HPP_
#define QORQL_SCISSORING_STRAIGHTENING_RANSACLINEFIT_HPP_

#include "../../qorql_IP.hpp"

using namespace cv;
using namespace std;

/*
This class is used to find the line fit of a set of points using RANSAC
The line is modeled as, ax+by=c.
*/
class RansacLineFit
{
private:
	vector<Point> data;
	//The line is modeled as ax+by=c.
	double a;
	double b;
	double c;
	//The threshold that determines whether a point is an inlier or not.
	double distThresh;
	double slope;
	double angle;
	double threshAngle;


public:
	RansacLineFit(vector<Point> data, double distThresh, double threshAngle)
	{
		this->data = data;
		this->a = 0;
		this->b = 0;
		this->c = 0;
		this->distThresh = distThresh;
		this->slope = 0;
		this->angle = 0;
		this->threshAngle = threshAngle;

		this->findBestLineFit();
	}

	void findBestLineFit()
	{
		int maxNoInliers = 0;
		for (unsigned int i = 0; i<data.size(); i++)
		{
			Point A = data[i];
			for (unsigned int j = i + 1; j<data.size(); j++)
			{
				Point B = data[j];

				double ta = -(A.y - B.y);
				double tb = (A.x - B.x);
				double tc = -((A.y - B.y)*A.x - (A.x - B.x)*A.y);
				if (abs(atan(tb / ta)) > threshAngle)
					continue;

				int noInliers = 0;
				for (unsigned int p = 0; p<data.size(); p++)
				{
					Point C = data[p];
					double dist = abs(ta*C.x + tb*C.y - tc) / sqrt(pow(ta, 2) + pow(tb, 2));

					if (dist<distThresh)
						noInliers++;
				}
				if (noInliers>maxNoInliers)
				{
					maxNoInliers = noInliers;
					a = ta;
					b = tb;
					c = tc;
				}
			}
		}
		slope = b / a;
		angle = -atan(slope)*(180 / CV_PI);
	}

	//Getter(s)
	double getA(){ return a; }
	double getB(){ return b; }
	double getC(){ return c; }
	double getSlope(){ return slope; }
	double getAngle(){ return angle; }
};

#endif //QORQL_SCISSORING_STRAIGHTENING_RANSACLINEFIT_HPP_
