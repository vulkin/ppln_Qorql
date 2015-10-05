#ifndef QORQL_STATISTICALRECOGNITION_RECOGNIZERS_LENGTHRECOGNIZER_HPP_
#define QORQL_STATISTICALRECOGNITION_RECOGNIZERS_LENGTHRECOGNIZER_HPP_

#include <opencv2/opencv.hpp>
#include "Recognizer.hpp"
#include "../../Features/Features.hpp"
#include "../Vocab/MetaWord.hpp"
#include "../Consolidator/SoftTag.hpp"

using namespace std;
using namespace cv;

/*
This is the LengthRecognizer.
It uses length to recognize the given word, using the vocabulary.
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
class LengthRecognizer : public Recognizer
{
	private:
		//Defines the prob range, that tells us whether the value is part of the given MetaWord or not.
		double threshProb;

	public:
		//Constructor(s)
		LengthRecognizer(vector<MetaWord> & vocab, double threshProb = 0.2) : Recognizer(vocab)
		{
			this->threshProb = threshProb;
		}

		//Function to recognize the given word and tag it
		vector<SoftTag> recognize(Word testWord)
		{
//			//TEST CODE
//			cout<<"In LengthRecognizer.recognize()<<endl";

			//Get the length
			vector<SoftTag> sTag;
			int testLength = Features::getLength(testWord);

			//Loop over the vocab
			for(unsigned int i=0; i<vocab.size(); i++)
			{
				vector<GaussianDistribution> lengthDistributions
					= vocab[i].getLengthDistributions();

//				double maxProb = Recognizer::getMaxBayesProb(testLength, lengthDistributions);
//				int maxProbIdx = Recognizer::getMaxBayesIdx(testLength, lengthDistributions);
				double maxProb = Recognizer::getMaxNormPdfProb(testLength, lengthDistributions);

				sTag.push_back
					(
						SoftTag
						(
							vocab.at(i).getTag(),
							maxProb,//lengthDistributions[maxProbIdx].getProbability(testLength),
							1//lengthDistributions[maxProbIdx].getN()
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
//			cout<<"In LengthRecognizer.update()"<<endl;

			//Get the tag and the length
			String tag = trainWord.getTag();
			int length = Features::getLength(trainWord);

//			//TEST CODE
//			cout<<"Length : "<<length<<endl;

			//If tag is empty, return.
			if(tag=="")
			{
				cout<<"In LengthRecognizer.update(), tag is empty"<<endl;
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

						vector<GaussianDistribution> lengthDistributions
							= vocab[i].getLengthDistributions();

						//If lengthDistributions isn't empty
						if(lengthDistributions.size()>0)
						{
							double maxProb = Recognizer::getMaxBayesProb(length, lengthDistributions);
							int maxProbIdx = Recognizer::getMaxBayesIdx(length,lengthDistributions);
//							double maxProb = Recognizer::getMaxNormPdfProb(length, lengthDistributions);
//							int maxProbIdx = Recognizer::getMaxNormPdfIdx(length, lengthDistributions);

							//If max probability is greater than threshold probability,
								//Update the maxProbIdx'th distribution distribution with length
//							if(maxProb>(threshProb/lengthDistributions.size()))
							if((maxProb>threshProb)&&(maxProb<1))
							{
								lengthDistributions[maxProbIdx].update(length);
								vocab[i].setLengthDistributions(lengthDistributions);
							}
							//Else, create a new distribution with length as the mean and zero stddev.
							else
							{
								lengthDistributions.push_back(GaussianDistribution(length));
								vocab[i].setLengthDistributions(lengthDistributions);
							}

						}
						else //If lengthDistributions is empty
						{
							//Add a new distribution to it.
							lengthDistributions.push_back(GaussianDistribution(length));
							//Update the current MetaWord
							vocab[i].setLengthDistributions(lengthDistributions);
						}
					}
				}
				if(!tagFound) //If tag was not found in vocab
				{
					//Create a new MetaWord
						MetaWord mw = MetaWord(tag);
					//Create a new lengthDistributions and add a new distribution to it.
						vector<GaussianDistribution> lengthDistributions;
						lengthDistributions.push_back(GaussianDistribution(length));
					//Update the MetaWord with the lengthDistributions.
						mw.setLengthDistributions(lengthDistributions);
					//Add the MetaWord to the vocab
						vocab.push_back(mw);
				}
			}
			else //If vocab is empty
			{
				//Create a new MetaWord
					MetaWord mw = MetaWord(tag);
				//Create a new lengthDistributions and add a new distribution to it.
					vector<GaussianDistribution> lengthDistributions;
					lengthDistributions.push_back(GaussianDistribution(length));
				//Update the MetaWord with the lengthDistributions.
					mw.setLengthDistributions(lengthDistributions);
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
		~LengthRecognizer(){;}
};

#endif //QORQL_STATISTICALRECOGNITION_RECOGNIZERS_CORRELATIONRECOGNIZER_HPP_
