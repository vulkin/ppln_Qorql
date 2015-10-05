#ifndef QORQL_STATISTICALRECOGNITION_RECOGNIZERS_CORRELATIONRECOGNIZER_HPP_
#define QORQL_STATISTICALRECOGNITION_RECOGNIZERS_CORRELATIONRECOGNIZER_HPP_

#include <opencv2/opencv.hpp>
#include "Recognizer.hpp"
#include "../Vocab/GaussianImage.hpp"
#include "../Vocab/MetaWord.hpp"

using namespace cv;

/*
This is the CorrelationRecognizer.
It uses correlation to recognize the given word, using the vocabulary.
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
class CorrelationRecognizer : public Recognizer
{
	private:
		//Defines the corr range, that tells us whether the value is part of the given MetaWord or not.
		double threshCorr;

	public:
		//Constructor(s)
		CorrelationRecognizer(vector<MetaWord> & vocab, double threshCorr = 0.8) : Recognizer(vocab)
		{
			this->threshCorr = threshCorr;
		}

		//Function to recognize the given word and tag it.
		vector<SoftTag> recognize(Word testWord)
		{
//			//TEST CODE
//			cout<<"In CorrelationRecognizer.recognize()<<endl";

//			vector<String> tags;
			vector<SoftTag> sTag;
			double currCorr;

			//Loop over the vocabulary
			for(unsigned int i=0; i<vocab.size(); i++)
			{
				vector<GaussianImage> corrDistributions
					= vocab[i].getCorrDistributions();

				double maxCorr = 0;
				//Loop over the vector<GaussianImage>
					//and find the GaussianImage with the max corr for the given image
				for(unsigned int j=0; j<corrDistributions.size(); j++)
				{
					currCorr = Features::getCorrelation
									(Word(corrDistributions.at(j).getMeanImage()), testWord);

					if(currCorr>maxCorr)
					{
						maxCorr=currCorr;
					}
				}

				sTag.push_back
					(
						SoftTag
						(
							vocab.at(i).getTag(),
							maxCorr,//maxCorr,
							1//corrDistributions[maxCorrIdx].getN()
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
//			cout<<"In CorrelationRecognizer.update()"<<endl;

			//Get the tag
			String tag = trainWord.getTag();
			double currCorr;

			//If tag is empty, return.
			if(tag=="")
			{
				cout<<"In CorrelationRecognizer.update(), tag is empty"<<endl;
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

						vector<GaussianImage> corrDistributions
							= vocab[i].getCorrDistributions();

						//If corrDistributions isn't empty
						if(corrDistributions.size()>0)
						{
							double maxCorr = 0;
							int maxCorrIdx = 0;
							//Loop over the vector<GaussianImage>
								//and find the GaussianImage with the max corr for the given image
							for(unsigned int j=0; j<corrDistributions.size(); j++)
							{
								currCorr = Features::getCorrelation
									(Word(corrDistributions.at(j).getMeanImage()), trainWord);
								if(currCorr>maxCorr)
								{
									maxCorr=currCorr;
									maxCorrIdx=j;
								}
							}

							//If max probability is greater than threshold probability,
								//Update the maxProbIdx'th distribution distribution with trainWord
							if(maxCorr>(threshCorr/corrDistributions.size()))
							{
								corrDistributions.at(maxCorrIdx).update(trainWord, maxCorr);
								vocab.at(i).setCorrDistributions(corrDistributions);
							}
							//Else, create a new distribution with trainWord
							else
							{
								corrDistributions.push_back(GaussianImage(trainWord.getImage()));
								vocab[i].setCorrDistributions(corrDistributions);
							}

						}
						else //If corrDistributions is empty
						{
							//Add a new distribution to it.
							corrDistributions.push_back(GaussianImage(trainWord.getImage()));
							//Update the current MetaWord
							vocab[i].setCorrDistributions(corrDistributions);
						}
					}
				}
				if(!tagFound) //If tag was not found in vocab
				{
					//Create a new MetaWord
						MetaWord mw = MetaWord(tag);
					//Create a new corrDistributions and add a new distribution to it.
						vector<GaussianImage> corrDistributions;
						corrDistributions.push_back(GaussianImage(trainWord.getImage()));
					//Update the MetaWord with the corrDistributions.
						mw.setCorrDistributions(corrDistributions);
					//Add the MetaWord to the vocab
						vocab.push_back(mw);
				}
			}
			else //If vocab is empty
			{
				//Create a new MetaWord
					MetaWord mw = MetaWord(tag);
				//Create a new corrDistributions and add a new distribution to it.
					vector<GaussianImage> corrDistributions;
					corrDistributions.push_back(GaussianImage(trainWord.getImage()));
				//Update the MetaWord with the corrDistributions.
					mw.setCorrDistributions(corrDistributions);
				//Add the MetaWord to the vocab
					vocab.push_back(mw);
			}

		}



};

#endif //QORQL_STATISTICALRECOGNITION_RECOGNIZERS_CORRELATIONRECOGNIZER_HPP_
