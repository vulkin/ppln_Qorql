#ifndef QORQL_SCISSORING_CROPPING_CROPPING_HPP_
#define QORQL_SCISSORING_CROPPING_CROPPING_HPP_

#include "../../qorql_IP.hpp"

using namespace cv;
using namespace std;

enum CropType
{
	CROP_ALL, //CROPS ALL EMPTY ROWS AND COLS. EVEN IF THEY ARE BETWEEN WORDS/LETTERS.
	CROP_EXTERNAL  //ONLY CROPS EMPTY ROWS/COLS AT THE EDGE OF THE IMAGE. DOES NOT CROP BETWEEN WORDS/LETTERS.

};

/*
This class contains static methods that will be used for cropping of the scissored input image.
The (perfect)cropping is done, so as to remove empty rows and columns.
*/
class Cropping
{
public:
	/*
	This function is to remove all rows and cols which have no black pixels.
	If input is RGB or GRAY, it will be thresholded to Binary for checking the no. of black pixels.
	This thresholding is done using THRESH_OTSU by default, but can be done at different level by passing it as "thresh"
	Even if thresholding is done, the output will still be the same format as input.

	Inputs
	Word input - The Word containing the Image to be processed.
	int thresh - If input is RGB/GRAY, this value is used for thresholding. Default value = OTSU.
	CropType   - Enum Specifying the type of Cropping.
	*/
	static Mat crop(Mat inputImage, int thresh = 0, CropType ct = CROP_EXTERNAL)
	{
		//Mat inputImage = input.getImage(); //Input image passed thorugh Word Object.
		Mat threshImage; //Thresholded Image used for checking no of black pixels.
		Mat tempImage; //Temp Image used in between eliminating empty rows and empty columns
		Mat outputImage; //Output Image that will be Returned as Word Object
		switch (ct)
		{
		case CROP_EXTERNAL:


			//Copy the inputImage into threshImage and tempImage
			threshImage = inputImage.clone();
			tempImage = inputImage.clone();

			//If its a 3 channel image, convert to single channel image
			if (threshImage.channels() == 3)
				cvtColor(threshImage, threshImage, CV_RGB2GRAY);

			//Threshold the single channel image
			threshold(threshImage, threshImage, thresh, 255, THRESH_BINARY | THRESH_OTSU);

			//Start from i=0.
			//If a row has a black pixel, add all rows after that row to tempImage
			for (int i = 0; i<threshImage.rows; i++)
			{
				if (threshImage.cols - (sum(threshImage.row(i))(0) / 255) > 0)
				{
					for (int j = i; j<threshImage.rows; j++)
						outputImage.push_back(Mat(tempImage.row(j)));
					break;
				}
			}

			//Copy the outputImage into threshImage and tempImage
			threshImage = outputImage.clone();
			tempImage = outputImage.clone();
			//Destroy the outputImage
			outputImage.release();
			//If its a 3 channel image, convert to single channel image
			if (threshImage.channels() == 3)
				cvtColor(threshImage, threshImage, CV_RGB2GRAY);
			//Threshold the single channel image
			threshold(threshImage, threshImage, thresh, 255, THRESH_BINARY | THRESH_OTSU);

			if (!threshImage.data)
				return Mat();
			if ((threshImage.rows == 0) || (threshImage.cols == 0))
				return Mat();


			//Start from i=threshImage.rows-1.
			//If a row has a black pixel, add all rows after that row to tempImage
			for (int i = threshImage.rows - 1; i >= 0; i--)
			{
				if (threshImage.cols - (sum(threshImage.row(i))(0) / 255) > 0)
				{
					for (int j = 0; j <= i; j++)
						outputImage.push_back(Mat(tempImage.row(j)));
					break;
				}
			}

			//Transpose, so that cols become rows. Then we can check for white columns by checking for white rows.
			outputImage = outputImage.t(); //Transpose the Matrix

			//Copy the outputImage into threshImage and tempImage
			threshImage = outputImage.clone();
			tempImage = outputImage.clone();

			//Destroy the outputImage
			outputImage.release();

			//If its a 3 channel image, convert to single channel image
			if (threshImage.channels() == 3)
				cvtColor(threshImage, threshImage, CV_RGB2GRAY);
			//Threshold the single channel image
			threshold(threshImage, threshImage, thresh, 255, THRESH_BINARY | THRESH_OTSU);

			//Start from i=0.
			//If a row has a black pixel, add all rows after that row to tempImage
			for (int i = 0; i<threshImage.rows; i++)
			{
				if (threshImage.cols - (sum(threshImage.row(i))(0) / 255) > 0)
				{
					for (int j = i; j<threshImage.rows; j++)
						outputImage.push_back(Mat(tempImage.row(j)));
					break;
				}
			}
			//Copy the outputImage into threshImage and tempImage
			threshImage = outputImage.clone();
			tempImage = outputImage.clone();
			//Destroy the outputImage
			outputImage.release();
			//If its a 3 channel image, convert to single channel image
			if (threshImage.channels() == 3)
				cvtColor(threshImage, threshImage, CV_RGB2GRAY);
			//Threshold the single channel image
			threshold(threshImage, threshImage, thresh, 255, THRESH_BINARY | THRESH_OTSU);
			//Start from i=threshImage.rows-1.
			//If a row has a black pixel, add all rows after that row to tempImage
			for (int i = threshImage.rows - 1; i >= 0; i--)
			{
				if (threshImage.cols - (sum(threshImage.row(i))(0) / 255) > 0)
				{
					for (int j = 0; j <= i; j++)
						outputImage.push_back(Mat(tempImage.row(j)));
					break;
				}
			}

			//Transpose, to get back the Image in proper orientation.
			outputImage = outputImage.t();

			//Return the output as a Word object
			return outputImage;



		}
	}
};

#endif //QORQL_SCISSORING_CROPPING_CROPPING_HPP_

