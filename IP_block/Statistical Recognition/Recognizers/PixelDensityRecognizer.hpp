#ifndef QORQL_STATISTICALRECOGNITION_RECOGNIZERS_PIXELDENSITYRECOGNIZER_HPP_
#define QORQL_STATISTICALRECOGNITION_RECOGNIZERS_PIXELDENSITYRECOGNIZER_HPP_

#include <opencv2/opencv.hpp>
#include "Recognizer.hpp"
#include "../../Features/Features.hpp"
#include "../Vocab/MetaWord.hpp"
#include "../Consolidator/SoftTag.hpp"

using namespace std;
using namespace cv;

/*
This is the PixelDensityRecognizer.
It uses pixel density to recognize the given word, using the vocabulary.
It also updates the vocabulary using a given word.
It has two functions.
 recognize(Word testWord)
 	 This function takes a Word object and returns a vector<SoftTag>.
 	 Each SoftTag object contains a tag from vocab, the probability that the given word is of that tag, and the no. of samples of that tag in the vocabulary.
 	 This vector is passed to a Consolidator, which uses this info, along with similar info from other Recognizers to decide the tag of the given Word Object.
 update(Word trainWord)
 	 This function takes a Word object and updates the vocabulary.
 	 The word must be tagged.
*/
class PixelDensityRecognizer : public Recognizer
{
	private:
		//Defines the prob range, that tells us whether the value is part of the given MetaWord or not.
		double threshProb;

	public:
		//Constructor(s)
		PixelDensityRecognizer(vector<MetaWord> & vocab, double threshProb = 0.2) : Recognizer(vocab)
		{
			this->threshProb = threshProb;
		}

		//Function to recognize the given word and tag it.
		vector<SoftTag> recognize(Word testWord)
		{
//			//TEST CODE
//			cout<<"In PixelDensityRecognizer.recognize()<<endl";

			//Get the aspectRatio
			vector<SoftTag> sTag;
			double testPixelDensity = Features::getPixelDensity(testWord);

			//Loop over the vocabulary
			for(unsigned int i=0; i<vocab.size(); i++)
			{
				vector<GaussianDistribution> pixelDensityDistributions
					= vocab[i].getPixelDensityDistributions();

//				double maxProb = Recognizer::getMaxBayesProb(testPixelDensity, pixelDensityDistributions);
//				int maxProbIdx = Recognizer::getMaxBayesIdx(testPixelDensity, pixelDensityDistributions);
				double maxProb = Recognizer::getMaxNormPdfProb(testPixelDensity, pixelDensityDistributions);

				sTag.push_back
					(
						SoftTag
						(
							vocab.at(i).getTag(),
							maxProb,//pixelDensityDistributions[maxProbIdx].getProbability(testPixelDensity),
							1//pixelDensityDistributions[maxProbIdx].getN()
						)
					);
			}
			//Return the tags of the recognized word
			return sTag;
		}

		//Function to update the vocab using the given word.
		void update(Word trainWord)
		{
//			//TEST CODE
//			cout<<"In PixelDensityRecognizer.update()"<<endl;

			//Get the tag and the pixelDensity
			String tag = trainWord.getTag();
			double pixelDensity = Features::getPixelDensity(trainWord);

//			//TEST CODE
//			cout<<"Pix Den : "<<pixelDensity<<endl;

			//If tag is empty, return.
			if(tag=="")
			{
				cout<<"In PixelDensityRecognizer.update(), tag is empty"<<endl;
				return;
			}

			//If vocab isn't empty
			if(vocab.size()>0)
			{
				bool tagFound = false; //true if tag is present in vocab
				//Loop over vocab
				for(unsigned int i=0; i<vocab.size(); i++)
				{
					//If we got a match between vocab and tag
					if(tag==vocab[i].getTag())
					{
						tagFound = true;

						vector<GaussianDistribution> pixelDensityDistributions
							= vocab[i].getPixelDensityDistributions();

						//If pixelDensityDistributions isn't empty
						if(pixelDensityDistributions.size()>0)
						{
							double maxProb = Recognizer::getMaxBayesProb(pixelDensity, pixelDensityDistributions);
							int maxProbIdx = Recognizer::getMaxBayesIdx(pixelDensity, pixelDensityDistributions);

							//If max probability is greater than threshold probability,
								//Update the maxProbIdx'th distribution distribution with pixelDensity
//							if(maxProb>(threshProb/pixelDensityDistributions.size()))
							if((maxProb>threshProb)&&(maxProb<1))
							{
								pixelDensityDistributions[maxProbIdx].update(pixelDensity);
								vocab[i].setPixelDensityDistributions(pixelDensityDistributions);
							}
							//Else, create a new distribution with aspectRatio as the mean and zero stddev.
							else
							{
								pixelDensityDistributions.push_back(GaussianDistribution(pixelDensity));
								vocab[i].setPixelDensityDistributions(pixelDensityDistributions);
							}

						}
						else //If aspectRatioDistributions is empty
						{
							//Add a new distribution to it.
							pixelDensityDistributions.push_back(GaussianDistribution(pixelDensity));
							//Update the current MetaWord
							vocab[i].setPixelDensityDistributions(pixelDensityDistributions);
						}
					}
				}
				if(!tagFound) //If tag was not found in vocab
				{
					//Create a new MetaWord
						MetaWord mw = MetaWord(tag);
					//Create a new aspectRatioDistributions and add a new distribution to it.
						vector<GaussianDistribution> pixelDensityDistributions;
						pixelDensityDistributions.push_back(GaussianDistribution(pixelDensity));
					//Update the MetaWord with the aspectRatioDistributions.
						mw.setPixelDensityDistributions(pixelDensityDistributions);
					//Add the MetaWord to the vocab
						vocab.push_back(mw);
				}
			}
			else //If vocab is empty
			{
				//Create a new MetaWord
					MetaWord mw = MetaWord(tag);
				//Create a new aspectRatioDistributions and add a new distribution to it.
					vector<GaussianDistribution> aspectRatioDistributions;
					aspectRatioDistributions.push_back(GaussianDistribution(pixelDensity));
				//Update the MetaWord with the aspectRatioDistributions.
					mw.setPixelDensityDistributions(aspectRatioDistributions);
				//Add the MetaWord to the vocab
					vocab.push_back(mw);
			}

		}

		//Allow the data science layer to modify the threshProb, if needed
		void setThreshProb(double threshProb)
		{
			this->threshProb = threshProb;
		}

		//Destructor
		~PixelDensityRecognizer(){;}
};

#endif //QORQL_STATISTICALRECOGNITION_RECOGNIZERS_PIXELDENSITYRECOGNIZER_HPP_
