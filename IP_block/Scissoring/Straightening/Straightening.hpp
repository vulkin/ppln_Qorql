#ifndef QORQL_SCISSORING_STRAIGHTENING_STRAIGHTENING_HPP_
#define QORQL_SCISSORING_STRAIGHTENING_STRAIGHTENING_HPP_

#include "../../qorql_IP.hpp"

using namespace cv;
using namespace std;

class Straightening
{
public:
	/*
	This function is to straighten the text in a scissored word and make it horizontal.
	If input is RGB or GRAY, it will be thresholded to Binary for processing.
	This thresholding is done using THRESH_OTSU by default, but can be done at different level by passing it as "thresh"
	Even if thresholding is done, the output will still be the same format as input.

	Inputs
	Word input - The Word object containing the image to be processed
	*/
	static Mat straighten(Mat inputImage, int thresh = 0, int ransacInlierThresh = 3)
	{
		//Mat inputImage = input.getImage(); //Input Image passed through the Word object
		Mat tempImage; //tempImage that is used for all processing
		Mat paddedImage; //Input image padded with more rows and cols
		Mat outputImage; //outputImage which is returned as a word object
		vector<Point> allContour;
		vector <Point> bottomContour; //Contour that contains points of the word that are closest to the bottom edge of the image.
		vector<Point> localMinima; //These are the point of the contour at which the "y" value is locally minimum. "y" is the distance between contour and bottom edge of the image.
		Vec4f rotationLine; //This is the line that gives the rotation angle.
		double rotationAngle; //The angle of the rotationLine
		Mat rotationMatrix; //The Rotation Matrix to be applied onto the Image.

		//Copy the inputImage into tempImage
		tempImage = inputImage.clone();

		//If its a 3 channel image, convert to single channel image
		if (tempImage.channels() == 3)
			cvtColor(tempImage, tempImage, CV_RGB2GRAY);

		//Threshold the single channel image
		threshold(tempImage, tempImage, thresh, 255, THRESH_BINARY | THRESH_OTSU);

		//Get the allContour
		for (int j = 0; j<tempImage.cols; j++) //Moving left to right
		{
			for (int i = tempImage.rows - 1; i >= 0; i--) //Moving bottom to top
			{
				if (tempImage.at<uchar>(i, j) == 0) //If we find a black pixel,
				{
					allContour.push_back(Point(i, j)); //We push it into bottomContour and break the inner loop.
				}
			}
		}


		RansacLineFit ransac = RansacLineFit(allContour, ransacInlierThresh, Features::getAspectRatio(Word(Cropping::crop(inputImage))) / 2);
		rotationAngle = ransac.getAngle();
		cout << "Rotated by : " << rotationAngle << endl;


		//Creating outputImage big enough to accommodate the rotated inputImage.
		int rows = inputImage.rows;
		int cols = inputImage.cols;
		int size = sqrt(pow(rows, 2) + pow(cols, 2));
		if (inputImage.channels() == 3)
		{
			paddedImage = Mat(size, size, CV_8UC3, Scalar(255, 255, 255));
			outputImage = Mat(size, size, CV_8UC3, Scalar(255, 255, 255));

			//Copying inputImage into outputImage concentrically.
			for (int i = (size - rows) / 2; i<(size - rows) / 2 + rows; i++)
			{
				int p = i - (size - rows) / 2;
				for (int j = (size - cols) / 2; j<(size - cols) / 2 + cols; j++)
				{
					int q = j - (size - cols) / 2;
					paddedImage.at<uchar>(i, j * 3) = inputImage.at<uchar>(p, q * 3);
					paddedImage.at<uchar>(i, j * 3 + 1) = inputImage.at<uchar>(p, q * 3 + 1);
					paddedImage.at<uchar>(i, j * 3 + 2) = inputImage.at<uchar>(p, q * 3 + 2);
				}
			}
		}
		else if (inputImage.channels() == 1)
		{
			paddedImage = Mat(size, size, CV_8UC1, Scalar(255));
			outputImage = Mat(size, size, CV_8UC1, Scalar(255));

			//Copying inputImage into outputImage concentrically.
			for (int i = (size - rows) / 2; i<(size - rows) / 2 + rows; i++)
			{
				int p = i - (size - rows) / 2;
				for (int j = (size - cols) / 2; j<(size - cols) / 2 + cols; j++)
				{
					int q = j - (size - cols) / 2;
					paddedImage.at<uchar>(i, j) = inputImage.at<uchar>(p, q);
				}
			}
		}

		//Getting the rotationMatrix from the slope.
		rotationMatrix =
			getRotationMatrix2D(Point(outputImage.size().height / 2, outputImage.size().width / 2), rotationAngle, 1);
		//Rotating the outputImage
		warpAffine(paddedImage, outputImage, rotationMatrix, Size(size, size), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));

		return outputImage;
	}

};

#endif //QORQL_SCISSORING_STRAIGHTENING_STRAIGHTENING_HPP_
