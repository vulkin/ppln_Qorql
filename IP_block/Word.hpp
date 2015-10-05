#ifndef WORD
#define WORD


//#ifndef WORD_HPPS
//#include <iostream>
//#include <vector>
//#include <fstream>
//#include <string>
//#include <math.h>
//#include <stdlib.h>
//#include <opencv2/opencv.hpp>
//#include "config_file.hpp"
//#endif
#include "qorql_IP.hpp"

using namespace cv;

//This class will be output type of scissoring and input type for tagging
class Word
{
private:
	Mat im; //Scissored Image
	Point origin; //Coordinates of top left corner of scissored part in prescription
	bool ok; //0 => scissoring was not done properly(im is of poor quality)
	cv::String tag;
	cv::String trueTag;


public:
	//Constructors
	Word(){ ok = false; }
	Word(Mat im)
	{
		this->im = im;
		this->origin = Point(0, 0);
		this->ok = true;
		this->tag = "";
		this->trueTag = "";
	}
	Word(Mat im, Point origin, bool ok)
	{
		this->im = im;
		this->origin = origin;
		this->ok = ok;
		this->tag = "";
		this->trueTag = "";
	}

	Word(Mat im, Point origin, bool ok, cv::String trueTag) : Word(im, origin, ok)
	{
		this->tag = "";
		this->trueTag = trueTag;
	}

	//Setter
	void appendTag(string tag)
	{
		if (this->tag == "")
		{
			this->tag.append(tag);
		}
		else
		{
			this->tag.append(",");
			this->tag.append(tag);
		}

	}

	void setImage(Mat img){ this->im; }

	//Getters
	Mat getImage(){ return im; }
	Point getOrigin(){ return this->origin; }
	bool imageOK(){ return this->ok; }
	cv::String getTag(){ return tag; }
	cv::String getTrueTag(){ return trueTag; }

	//Resize
	void resizeImage(Size size, int interpolationMethod = INTERPOLATION)
	{
		resize(im, im, size, 0, 0, interpolationMethod);
	}
};



#endif
