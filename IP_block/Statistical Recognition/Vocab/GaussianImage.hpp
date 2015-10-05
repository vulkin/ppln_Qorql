#ifndef STATISTICALRECOGNITION_VOCAB_GAUSSIANIMAGE_HPP_
#define STATISTICALRECOGNITION_VOCAB_GAUSSIANIMAGE_HPP_

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

/*
This class represents a Gaussian Image.

What is a Gaussian Image?
Consider a set of "n" images of dimension "p*q".
Consider a pixel at (i,j) to be an RV over the set of images.
Now, the value of this pixel can be modeled as a gaussian distribution.
Thus, we crunch the set of "n" images, into a set of "p*q" gaussians.
Now the "p*q" gaussians are completely defined by their parameters, mean and stddev.
Thus, we define meanImage and stddevImage, which are "p*q" sized matrices that hold,
	the mean and stdddev of of all "p*q" distributions.

What is meanCorr?
Essentially, this class represents a set of "n" images in the form of meanImage and stddevImage.
meanCorr is the mean of the cross-correlation of these "k" images.
Thus, meanCorr was calculated from a dataset of "(n)*(n-1)/2" cross-correlation values.

What is n?
It is the no of images.

What is update(Mat inputImage, double corr)?
It is a function to add a new image to the set, and update the gaussian image, mean corr etc.
*/
class GaussianImage
{
	private:
		Mat meanImage;
		Mat stddevImage;
		double meanCorr;
		int n;

	public:
		//Constructor(s)
		GaussianImage(Mat meanImage, Mat stddevImage, double meanCorr, int n)
		{
			this->meanImage=meanImage;
			this->stddevImage=stddevImage;
			this->meanCorr=meanCorr;
			this->n=n;
		}
		GaussianImage(Mat meanImage)
		{
			this->meanImage = meanImage;
			this->stddevImage = Mat::ones(meanImage.size(), meanImage.type());
			this->n = 1;
			this->meanCorr = 0;
		}

		//Getter(s)
		Mat getMeanImage(){return meanImage;}
		Mat getStddevImage(){return stddevImage;}
		double getMeanCorr(){return meanCorr;}
		int getN(){return n;}

		//Updater
		void update(Word inputWord, double corr)
		{
			Mat inputImage = inputWord.getImage();

			//Updating meanImage
			Mat newMeanImage;
			addWeighted(meanImage,(double)n/(n+1),inputImage,(double)1/(n+1),0,newMeanImage);
//			Mat newMeanImage = (meanImage*n+inputImage)/n+1;

			//Updating stddevImage
			Mat stddevImageSq,meanImageSq,inputImageSq,newMeanImageSq,newStddevImage;
			stddevImage.convertTo(stddevImageSq, CV_32F);
			meanImage.convertTo(meanImageSq,CV_32F);
			inputImage.convertTo(inputImageSq,CV_32F);
			newMeanImage.convertTo(newMeanImageSq,CV_32F);
			pow(stddevImageSq, 2, stddevImageSq);
			pow(meanImageSq, 2, meanImageSq);
			pow(inputImageSq,2,inputImageSq);
			pow(newMeanImageSq ,2,newMeanImageSq);
			newStddevImage = ( (n*(stddevImageSq+meanImageSq)+inputImageSq)/n+1 ) - newMeanImageSq;
			sqrt(newStddevImage, newStddevImage);

			//Updating meanCorr
			double newMeanCorr = ( meanCorr*n*(n-1)/2 + corr*n )/((double)n*(n+1)/2);

			//Copying the updated values
			meanImage = newMeanImage;
			stddevImage = newStddevImage;
			meanCorr = newMeanCorr;
			n=n+1;
		}
};

#endif //STATISTICALRECOGNITION_VOCAB_GAUSSIANIMAGE_HPP_
