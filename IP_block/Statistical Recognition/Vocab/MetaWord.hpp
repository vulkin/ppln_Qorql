#ifndef STATISTICALRECOGNITION_VOCAB_METAWORD_HPP_
#define STATISTICALRECOGNITION_VOCAB_METAWORD_HPP_

#include<opencv2/opencv.hpp>
#include<iostream>

#include "GaussianDistribution.hpp"
#include "GaussianImage.hpp"

using namespace cv;
using namespace std;

/*
This class is a MetaWord. It contains the Meta of a Word in the vocabulary.
It contains a tag, along with the distributions of various features of that tag.
A vector of objects of this class is used by the Recognizers for recognition.
*/
class MetaWord
{
	private:
		String tag;
		vector<GaussianDistribution> aspectRatioDistributions;
		vector<GaussianDistribution> lengthDistributions;
		vector<GaussianDistribution> pixelDensityDistributions;
		vector<GaussianImage> corrDistributions;

	public:
		//Constructor
		MetaWord(String tag)
		{
			this->tag = tag;
		}

		//Getter(s)
		String getTag(){return tag;}
		vector<GaussianDistribution> getAspectRatioDistributions(){return aspectRatioDistributions;}
		vector<GaussianDistribution> getLengthDistributions(){return lengthDistributions;}
		vector<GaussianDistribution> getPixelDensityDistributions(){return pixelDensityDistributions;}
		vector<GaussianImage> getCorrDistributions(){return corrDistributions;}

		//Setter(s)
		void setAspectRatioDistributions(vector<GaussianDistribution> aspectRatioDistributions)
		{this->aspectRatioDistributions = aspectRatioDistributions;}
		void setLengthDistributions(vector<GaussianDistribution> lengthDistributions)
		{this->lengthDistributions = lengthDistributions;}
		void setPixelDensityDistributions(vector<GaussianDistribution> pixelDensityDistributions)
		{this->pixelDensityDistributions = pixelDensityDistributions;}
		void setCorrDistributions(vector<GaussianImage> corrDistributions)
		{this->corrDistributions = corrDistributions;}
};

#endif //STATISTICALRECOGNITION_VOCAB_METAWORD_HPP_
