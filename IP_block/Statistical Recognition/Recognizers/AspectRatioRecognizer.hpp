#ifndef QORQL_STATISTICALRECOGNITION_RECOGNIZERS_ASPECTRATIORECOGNIZER_HPP_
#define QORQL_STATISTICALRECOGNITION_RECOGNIZERS_ASPECTRATIORECOGNIZER_HPP_

#include <opencv2/opencv.hpp>
#include "Recognizer.hpp"
#include "../../Features/Features.hpp"
#include "../Vocab/MetaWord.hpp"
#include "../Consolidator/SoftTag.hpp"

using namespace cv;

/*
This is the AspectRatioRecognizer.
It uses aspect ratio to recognize the given word, using the vocabulary.
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
class AspectRatioRecognizer : public Recognizer
{
	private:
		//Defines the prob range, that tells us whether the value is part of the given MetaWord or not.
		double threshProb;

	public:
		//Constructor(s)
		AspectRatioRecognizer(vector<MetaWord> & vocab, double threshProb = 0.2) : Recognizer(vocab)
		{
			this->threshProb = threshProb;
		}

		//Function to recognize the given word and tag it.
		vector<SoftTag> recognize(Word testWord)
		{
//			//TEST CODE
//			cout<<"In AspectRatioRecognizer.recognize()<<endl";

			//Get the aspectRatio
			vector<SoftTag> sTag;
			double testAspectRatio = Features::getAspectRatio(testWord);

			//Loop over the vocabulary
			for(unsigned int i=0; i<vocab.size(); i++)
			{
				vector<GaussianDistribution> aspectRatioDistributions
					= vocab[i].getAspectRatioDistributions();

//				double maxProb = Recognizer::getMaxBayesProb(testAspectRatio, aspectRatioDistributions);
//				int maxProbIdx = Recognizer::getMaxBayesIdx(testAspectRatio, aspectRatioDistributions);
				double maxProb = Recognizer::getMaxNormPdfProb(testAspectRatio, aspectRatioDistributions);

				sTag.push_back
					(
						SoftTag
						(
							vocab.at(i).getTag(),
							maxProb,//aspectRatioDistributions[maxProbIdx].getProbability(testAspectRatio),
							1//aspectRatioDistributions[maxProbIdx].getN()
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
//			cout<<"In AspectRatioRecognizer.update()"<<endl;

			//Get the tag and the aspectRatio
			String tag = trainWord.getTag();
			double aspectRatio = Features::getAspectRatio(trainWord);

//			//TEST CODE
//			cout<<"Asp : "<<aspectRatio<<endl;

			//If tag is empty, return.
			if(tag=="")
			{
				cout<<"In AspectRatioRecognizer.update(), tag is empty"<<endl;
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

						vector<GaussianDistribution> aspectRatioDistributions
							= vocab[i].getAspectRatioDistributions();

						//If aspectRatioDistributions isn't empty
						if(aspectRatioDistributions.size()>0)
						{
							int maxProbIdx = Recognizer::getMaxBayesIdx(aspectRatio, aspectRatioDistributions);
							double maxProb = Recognizer::getMaxBayesProb(aspectRatio,aspectRatioDistributions);
//							int maxProbIdx = Recognizer::getMaxNormPdfIdx(aspectRatio, aspectRatioDistributions);
//							double maxProb = Recognizer::getMaxNormPdfProb(aspectRatio, aspectRatioDistributions);

							//If max probability is greater than threshold probability,
								//Update the maxProbIdx'th distribution distribution with aspectRatio
//							if(maxProb>(threshProb/aspectRatioDistributions.size()))
							if((maxProb>threshProb)&&(maxProb<1))
							{
								aspectRatioDistributions[maxProbIdx].update(aspectRatio);
								vocab[i].setAspectRatioDistributions(aspectRatioDistributions);
							}
							//Else, create a new distribution with aspectRatio as the mean and zero stddev.
							else
							{
								aspectRatioDistributions.push_back(GaussianDistribution(aspectRatio));
								vocab[i].setAspectRatioDistributions(aspectRatioDistributions);
							}

						}
						else //If aspectRatioDistributions is empty
						{
							//Add a new distribution to it.
							aspectRatioDistributions.push_back(GaussianDistribution(aspectRatio));
							//Update the current MetaWord
							vocab[i].setAspectRatioDistributions(aspectRatioDistributions);
						}
					}
				}
				if(!tagFound) //If tag was not found in vocab
				{
					//Create a new MetaWord
						MetaWord mw = MetaWord(tag);
					//Create a new aspectRatioDistributions and add a new distribution to it.
						vector<GaussianDistribution> aspectRatioDistributions;
						aspectRatioDistributions.push_back(GaussianDistribution(aspectRatio));
					//Update the MetaWord with the aspectRatioDistributions.
						mw.setAspectRatioDistributions(aspectRatioDistributions);
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
					aspectRatioDistributions.push_back(GaussianDistribution(aspectRatio));
				//Update the MetaWord with the aspectRatioDistributions.
					mw.setAspectRatioDistributions(aspectRatioDistributions);
				//Add the MetaWord to the vocab
					vocab.push_back(mw);
			}

		}

		//Allow the data science layer to modify the threshProb, if needed
		void setThreshProb(double threshProb)
		{
			this->threshProb = threshProb;
		}

		vector<MetaWord> getVocab(){return vocab;}

		//Destructor
		~AspectRatioRecognizer(){;}
};

#endif //QORQL_STATISTICALRECOGNITION_RECOGNIZERS_ASPECTRATIORECOGNIZER_HPP_
