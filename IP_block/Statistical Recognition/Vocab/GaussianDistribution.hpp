#ifndef STATISTICALRECOGNITION_VOCAB_GAUSSIANDISTRIBUTION_HPP_
#define STATISTICALRECOGNITION_VOCAB_GAUSSIANDISTRIBUTION_HPP_

#include <boost/math/special_functions/erf.hpp>
#include <math.h>
#include <typeinfo>

using namespace boost;
using namespace std;

/*
This class represents a Scalar Gaussian Distribution with members,
"mean" - mean of distribution.
"stddev" - std. deviation of distribution.
"n" - no. of elements in the distribution.
"max" - max sample value in the sample set used to create the distribution.
"getProbability(double value)" - gives the pdf at value.
"update(double value)" - function to update the distribution with new element "value"
	NOTE on update() :
		We dont store all sample values of the distribution.
		Every time a sample value is given, we use it to update the mean and stddev, and then we drop the sample.
		This is done by using a modified version of Sample mean and Sample variance formulae.
		newmean = (mean*n+value)/(n+1);
		newstddev = sqrt( ( (n*(pow(stddev,2)+pow(mean,2)) + pow(value,2))/(n+1) - pow(newmean,2) ) );

NOTE :
	Since we initialize a new distribution with stddev = 1, the samples range must be invariant.
	Thus we have to scale aspect ratio:0<->1 and length:0<->200 to same scale (The ranges given before were just examples)
	We do this by dividing the samples, by max(sample values), so that the range of sample values becomes 0<->1.
	Since we dont store all the sample points, we implement the scaling, by simply saving "max".
*/
class GaussianDistribution
{
	private:
		double mean;
		double stddev;
		int n;
		double max;
	public:
		//Constructor(s)
		GaussianDistribution(double mean, double stddev = 1, int n = 1)
		{
			//Scaling the Distribution so that mean becomes 1.
			this->max = mean;
			mean=1;

			this->mean=mean;
			this->stddev=stddev;
			this->n=n;
		}

		//Getter(s)
		double getMean(){return mean*max;}
		double getStddev(){return stddev*max;}
		double getMax(){return max;}
		int getN(){return n;}
		double getProbability(double value)
		{
			//Since we have a scaled distribution, we need to get the prob of scaled value.
			value/=max;

			//prob is the PDF of the gaussian at value.
			double prob = (1/(sqrt(2*CV_PI)*stddev)) * exp(-pow(((value-mean)/(sqrt(2)*stddev)),2));
			return prob;
		}

		//Updater
		void update(double value)
		{
			//If value is more than max,
			//We need to change the scale.
			if(value>max)
			{
				this->mean = mean*max/value;
				this->stddev = stddev*max/value;
				max=value;
				value=1;
			}
			//Else, we need to update with scaled value.
			else
			{
				value/=max;
			}

			double newmean = (mean*n+value)/(n+1);
			double newstddev
				= sqrt( ( (n*(pow(stddev,2)+pow(mean,2)) + pow(value,2))/(n+1) - pow(newmean,2) ) );
//			//TEST CODE
//			cout<<"Old mean, stddev and n : "<<mean<<","<<stddev<<","<<n<<endl;
//			cout<<"Value given : "<<value<<endl;
//			cout<<"New mean, stddev and n : "<<newmean<<","<<newstddev<<","<<n+1<<endl;
//			//TEST CODE ENDS
			mean = newmean;
			stddev = newstddev;
			n++;
		}

};


#endif //STATISTICALRECOGNITION_VOCAB_GAUSSIANDISTRIBUTION_HPP_
