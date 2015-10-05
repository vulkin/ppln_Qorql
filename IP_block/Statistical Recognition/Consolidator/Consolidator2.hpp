#ifndef QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_CONSOLIDATOR2_HPP_
#define QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_CONSOLIDATOR2_HPP_

#include <vector>
#include <fstream>
#include <iomanip>
#include "SoftTag.hpp"

using namespace std;

/*
Consolidator2 is another version of consolidator.
In this, we use AspectRatioRecognizer, LengthRecognizer and PixelDensityRecognizer as filter to weed out the wrong tags.
Then finally, the tag is decided by the CorreleationRecognizer.
The process :
	The vector<SoftTag> for the aspect ratio is taken. Its is sorted according to the probability and the top half tags are taken.
	Then, the vector<SoftTag> of these tags are taken for the length. Its is sorted according to probability, and the top half tags are taken again.
	We do the same process for pixel density.
	Finally, we decide the tag using the correlation of the remaining tags.
	The tag with highest correlation is decided to be tag of the given word.
 */
class Consolidator2
{
	private:
		vector<vector<SoftTag> > softTags;
		String tag;
		String trueTag;

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

			vector<SoftTag> possibleTags;
			SoftTag temp;
			bool mightExist = true;
			for(unsigned int i=0; i<softTags.size(); i++)
			{
				vector<SoftTag> currRec;
				if(i!=0)
				{
					vector<SoftTag> temp = softTags.at(i);
					for(unsigned int a=0; a<possibleTags.size(); a++)
					{
						for(unsigned int b=0; b<temp.size(); b++)
						{
							if(possibleTags[a].getTag()==temp[b].getTag())
								currRec.push_back(temp[b]);
						}
					}
					possibleTags.clear();
				}
				else
				{
					currRec = softTags.at(i);
				}

				if(mightExist)
				{
					bool exists=false;
					for(unsigned int c=0; c<currRec.size(); c++)
					{
						if(currRec[c].getTag()==trueTag)
							exists = true;
					}
					if(!exists)
					{
						mightExist=false;
						if(i!=0)
							cout<<"MISSED AT : "<<i-1<<endl;
						else
							cout<<"MISSED AT : NOT IN DB"<<endl;
					}
				}

				SoftTag tempProb;
				for(unsigned int p=0; p<currRec.size(); p++)
				{
					for(unsigned int q=p; q<currRec.size(); q++)
					{
						if(currRec[p].getProb()<currRec[q].getProb())
						{
							tempProb = currRec[p];
							currRec[p] = currRec[q];
							currRec[q] = tempProb;
						}
					}
				}

				int idx = 0;
				double currDiff, maxDiff = 0;
				for(unsigned int j=0; j<currRec.size()-1; j++)
				{
					currDiff = currRec[j].getProb()-currRec[j+1].getProb();
//					cout<<currDiff;
					if(currDiff>maxDiff)
					{
						maxDiff = currDiff;
						idx = j;
					}
				}

//				int idx = currRec.size()/2;

				for(int j=0; j<=idx; j++)
					possibleTags.push_back(currRec.at(j));

				for(unsigned int j=0; j<possibleTags.size(); j++)
					cout<<"\t\t\t"<<i<<"\t"<<possibleTags.at(j).getTag()<<"\t"<<possibleTags.at(j).getProb()<<endl;
				cout<<endl;
			}

//			for(unsigned int i=0; i<possibleTags.size(); i++)
//			{
//				cout<<possibleTags[i].getTag()<<endl;
//			}
//			cout<<endl;

			tag = possibleTags[0].getTag();
		}

	public:
		Consolidator2(vector<vector<SoftTag> > softTags, String trueTag)
		{
			this->softTags = softTags;
			this->trueTag = trueTag;
			this->consolidate();
		}

		String getTag(){return tag;}
};





#endif //QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_CONSOLIDATOR_HPP_
