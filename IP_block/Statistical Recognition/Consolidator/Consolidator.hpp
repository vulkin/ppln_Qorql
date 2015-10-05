#ifndef QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_CONSOLIDATOR_HPP_
#define QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_CONSOLIDATOR_HPP_

#include <vector>
#include <fstream>
#include <iomanip>
#include "SoftTag.hpp"

using namespace std;

/*
Consolidator consolidates the information given by various recognizers, and gives a tag as output.
The input is as follows:
	The pdf values for the given Word, for all Tags in the vocabulary, from all Recognizers
	softTags is a vector of vector of SoftTag. The outer vector gives the different Recognizers, while the inner vector gives different tags.
	The final SoftTag object contains a <tag,pdf,n>.
The output is a tag.
The process is as follows :
	For each recognizer, we take the probabilities of different tags, along with their respective "n", to calculate the bayesian probability.
	We do this for all Recognizers, to get bayesSoftTags.
	We do bayesian for two reasons :
		1) So that probabilities of all recognizers are normalized from 0-1. PDF values, are obviously not normalized.
		2) So that class size "n" will also be a factor in deciding the tag of the given Word.
	Now that we have the bayesSoftTags, we multiply the probability of different recognizers, for every tag.
	Thus we get a list of probabilities, and the tag of the word is decided to be the tag with the maximum probability.

Of course, by doing this, we are assuming that the probabilites given by different recognizers are independent.
But that assumption has to be made in Naive Bayes Classification.
 */
class Consolidator
{
	private:
		vector<vector<SoftTag> > softTags;
		vector<vector<SoftTag> > bayesSoftTags;
		String tag;

		void consolidate()
		{
			//Check if softTags is empty and return
			if(softTags.size()==0)
			{
				tag = "softTags Empty";
				return;
			}


			//Check if vector<SoftTag> given by all Rec is of same length.
			//It should be same, because they share common vocab.
			bool check = true;
			unsigned int size = softTags.at(0).size();
			for(unsigned int i=1; i<softTags.size(); i++)
			{
				if(size!=softTags.at(i).size())
					check = false;
			}
			if(!check)
			{
				tag = "size of softTags.at(i) is not same for all i";
				return;
			}

			//Calculating bayesSoftTags
			for(unsigned int i=0; i<softTags.size(); i++) //Looping over the recognizers
			{
				vector<SoftTag> currRec = softTags.at(i);

				int totalSize = 0;
				for(unsigned int j=0; j<currRec.size(); j++)
				{
					totalSize += currRec.at(j).getSize();
				}

				double totalProb = 0;
				for(unsigned int j=0; j<currRec.size(); j++)
				{
					totalProb += currRec.at(j).getProb() * ((double)currRec.at(j).getSize()/totalSize);
				}

				vector<SoftTag> bayesCurrRec;
				bayesCurrRec.resize(currRec.size());
				for(unsigned int j=0; j<currRec.size(); j++)
				{
					bayesCurrRec.at(j) =
							SoftTag
							(
								currRec.at(j).getTag(),
								currRec.at(j).getProb() * ((double)currRec.at(j).getSize()/totalSize) / totalProb,
								1
							);
				}

				bayesSoftTags.at(i) = bayesCurrRec;
			}

			double prodProb;
			double maxProdProb = 0;
			int idx = 0;
			for(unsigned int i=0; i<bayesSoftTags.at(0).size(); i++)
			{
				prodProb = 1;
				for(unsigned int j=0; j<bayesSoftTags.size(); j++)
				{
					prodProb *= bayesSoftTags.at(j).at(i).getProb();
				}
				if(prodProb>maxProdProb)
				{
					maxProdProb = prodProb;
					idx = i;
				}
			}


			tag = softTags.at(0).at(idx).getTag();
		}

	public:
		Consolidator(vector<vector<SoftTag> > softTags)
		{
			this->softTags = softTags;
			this->bayesSoftTags.resize(softTags.size());
			this->consolidate();
		}

		String getTag(){return tag;}
};


#endif //QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_CONSOLIDATOR_HPP_
