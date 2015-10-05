#ifndef QORQL_FEATURES_FEATURES_HPP
#define QORQL_FEATURES_FEATURES_HPP

#include "../qorql_IP.hpp"


using namespace cv;
using namespace std;

/*
This class is used to contain static methods that give us various features.
The features implemented :
Pixel Density
Aspect Ratio
Length
Correlation
Row Pixel Density
Column Pixel Density
*/
class Features
{
public:
	//Pixel Density = (No. of white pixels/No. of black pixels)
	//Its not a very good feature because of one reason
	//Motion Blur will drastically affect this feature.
	static double getPixelDensity(Word inputWord, int thresh = 0)
	{
		//Get the inputImage from the inputWord
		Mat inputImage = inputWord.getImage();

		//If its a 3 channel image, convert to single channel image
		if (inputImage.channels() == 3)
			cvtColor(inputImage, inputImage, CV_RGB2GRAY);

		//Threshold the single channel image
		threshold(inputImage, inputImage, thresh, 255, THRESH_BINARY | THRESH_OTSU);

		int nWhite = 0;
		int nBlack = 0;
		for (int i = 0; i<inputImage.rows; i++)
		{
			for (int j = 0; j<inputImage.cols; j++)
			{
				if (inputImage.at<uchar>(i, j) == 255)
					nWhite++;
				else if (inputImage.at<uchar>(i, j) == 0)
					nBlack++;
				else
					cout << "ERROR in getPixelDensity : Image is not Binary" << endl;
			}
		}

		return ((double)nWhite / nBlack);
	}

	static vector<double> getPixelDensity(vector<Word> inputWords, int thresh = 0)
	{
		vector<double> pixelDensities;
		pixelDensities.reserve(inputWords.size());
		for (unsigned int i = 0; i<inputWords.size(); i++)
			pixelDensities.at(i) = getPixelDensity(inputWords.at(i), thresh);

		return pixelDensities;
	}

	//Aspect Ratio = (Width/Height)
	//This is a pretty decent feature
	static double getAspectRatio(Word inputWord)
	{
		Mat inputImage = inputWord.getImage();
		return ((double)inputImage.size().width / inputImage.size().height);
	}

	static vector<double> getAspectRatio(vector<Word> inputWords)
	{
		vector<double> aspectRatios;
		aspectRatios.reserve(inputWords.size());
		for (unsigned int i = 0; i<inputWords.size(); i++)
			aspectRatios.at(i) = getAspectRatio(inputWords.at(i));

		return aspectRatios;
	}

	//Length of the word, counted as no. of pixels(after cropping)
	static int getLength(Word inputWord)
	{
		return inputWord.getImage().size().width;
	}

	static vector<int> getLength(vector<Word> inputWords)
	{
		vector<int> length;
		length.reserve(inputWords.size());
		for (unsigned int i = 0; i<inputWords.size(); i++)
			length.at(i) = getLength(inputWords.at(i));

		return length;
	}

	//Digital Image Correlation. Search google for exact formula.
	static double getCorrelation(Word X, Word Y, int thresh = 0)
	{
		Mat A = X.getImage();//.clone();
		Mat B = Y.getImage();//.clone();
		if (A.channels() == 3)
			cvtColor(A, A, CV_RGB2GRAY);
		if (B.channels() == 3)
			cvtColor(B, B, CV_RGB2GRAY);

		threshold(A, A, thresh, 255, THRESH_BINARY | THRESH_OTSU);
		threshold(B, B, thresh, 255, THRESH_BINARY | THRESH_OTSU);

		Mat corr;
		matchTemplate(A, B, corr, TEMP_MATCH_METHOD);
		//		//TEST CODE
		//		namedWindow("InGetCorr1",CV_WINDOW_NORMAL);
		//		imshow("InGetCorr1",A);
		//		namedWindow("InGetCorr2",CV_WINDOW_NORMAL);
		//		imshow("InGetCorr2",B);
		//		cout<<corr.at<float>(0,0)<<endl;
		//		waitKey(0);
		return corr.at<float>(0, 0);
	}

	static vector<double> getCorrelation(Word inputWord, vector<Word> meta)
	{
		vector<double> corr;

		for (unsigned int i = 0; i<meta.size(); i++)
		{
			corr.at(i) = getCorrelation(inputWord, meta.at(i));
		}

		return corr;
	}

	//A column vector that contains the sum of every row in the image.
	static vector<double> getRowPixelDensity(Word inputWord)
	{
		Mat inputImage = inputWord.getImage().clone();
		if (inputImage.channels() == 3)
			cvtColor(inputImage, inputImage, CV_RGB2GRAY);

		Mat reducedImage = Mat(inputImage.size(), CV_64F);
		reduce(inputImage, reducedImage, 1, REDUCTION_METH, CV_64F);

		vector<double> rowPixelDensity;
		rowPixelDensity.assign((double*)reducedImage.datastart, (double*)reducedImage.dataend);

		return rowPixelDensity;
	}

	//A row vector that contains the sum of every column in the image.
	static vector<double> getColPixelDensity(Word inputWord)
	{
		Mat inputImage = inputWord.getImage().clone();
		if (inputImage.channels() == 3)
			cvtColor(inputImage, inputImage, CV_RGB2GRAY);

		Mat reducedImage = Mat(inputImage.size(), CV_64F);
		reduce(inputImage, reducedImage, 0, REDUCTION_METH, CV_64F);

		vector<double> colPixelDensity;
		colPixelDensity.assign((double*)reducedImage.datastart, (double*)reducedImage.dataend);

		return colPixelDensity;
	}

	//The HOG feature Extractor
	static vector<double> getHOG(Word inputWord)
	{
		Mat inputImage = inputWord.getImage();

		HOGDescriptor hog(Size(16, 16), Size(16, 16), Size(16, 16), Size(16, 16), 9);

		vector<float> temp;
		vector<Point>locations;
		vector<double>hogFeatures;
		hog.compute(inputImage, temp, Size(0, 0), Size(0, 0), locations);

		for (int i = 0; i<temp.size(); i++)
		{
			hogFeatures.push_back((double)temp[i]);
		}

		return hogFeatures;
	}


	// //Get HOG features from given Image
	// vector<double> ReducedextractHOGfet1(Mat img)
	// {
	// 	HOGDescriptor hog( Size(16,16), Size(16,16), Size(16,16), Size(16,16),9); 
	// 	// Size(32,16), //winSize  
	// 		// Size(8,8), //blocksize  
	// 		// Size(4,4), //blockStride,  
	// 		// Size(4,4), //cellSize,  
	// 		// 9, //nbins, 

	// 	vector<float> ders;
	// 	vector<Point>locs;
	// 	vector<double>res;
	// 	hog.compute(img,ders,Size(0,0),Size(0,0),locs);

	// 	for(int i=0;i<ders.size()/9;i++)
	// 	{
	// 		float Htem=0.0;
	// 		for(int j=0;j<9;j++)
	// 		{	
	// 			Htem+=ders[9*i+j];
	// 		}
	// 		Htem/=9;
	// 		res.push_back((double)Htem);
	// 		//cout<<res[i]<<endl;
	// 	}

	// 	return res;
	// }


	// //Get HOG features from given Image
	// vector<double> ReducedextractHOGfet2(Mat img)
	// {
	// 	HOGDescriptor hog( Size(16,16), Size(16,16), Size(16,16), Size(16,16),9); 
	// 	// Size(32,16), //winSize  
	// 		// Size(8,8), //blocksize  
	// 		// Size(4,4), //blockStride,  
	// 		// Size(4,4), //cellSize,  
	// 		// 9, //nbins, 
	// 	float variance=0.5;

	// 	vector<float> ders;
	// 	vector<Point>locs;
	// 	vector<double>res;
	// 	hog.compute(img,ders,Size(0,0),Size(0,0),locs);



	// 	for(int i=0;i<ders.size()/9;i++)
	// 	{
	// 		vector<float> red;
	// 		float Htem=0.0;
	// 		for(int j=0;j<9;j++)
	// 		{	
	// 			red.push_back(ders[9*i+j]);
	// 		}
	// 		sort(red.begin(),red.end());

	// 		int ind_beg=0,ind_end=0,max_beg=0,max_end=0;
	// 		for(int j=1;j<9;j++)
	// 		{
	// 			if(abs(red[j]-red[j-1])<variance)
	// 			{
	// 				ind_end=j;
	// 			}
	// 			else
	// 			{
	// 				if((max_end-max_beg)<(ind_end-ind_beg))
	// 				{
	// 					max_beg=ind_beg;
	// 					max_end=ind_end;
	// 				}
	// 				ind_beg=j;
	// 				ind_end=j;
	// 			}
	// 		}
	// 		int count=0;
	// 		float mean=0.0;
	// 		for(int j=max_beg;j<=max_end;j++)
	// 		{
	// 			mean+=red[j];
	// 			count++;
	// 		}
	// 		mean/=count;

	// 		res.push_back((double)mean);
	// 		//cout<<res[i]<<endl;
	// 	}

	// 	return res;
	// }
};

#endif //QORQL_FEATURES_FEATURES_HPP
