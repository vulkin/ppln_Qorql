#ifndef QORQL_STATISTICALRECOGNITION_RECOGNIZERS_RECOGNIZER_HPP_
#define QORQL_STATISTICALRECOGNITION_RECOGNIZERS_RECOGNIZER_HPP_

#include "../Vocab/MetaWord.hpp"
#include "../Consolidator/SoftTag.hpp"

/*
This class is used basically for abstraction of all recognizers.
*/
class Recognizer
{
	protected:
		vector<MetaWord> & vocab;

	public:
		Recognizer(vector<MetaWord> & vocab) : vocab(vocab){;}

		virtual vector<SoftTag> recognize(Word testWord) = 0;
		virtual void update(Word trainWord) = 0;
		virtual ~Recognizer(){;}

		//FUNCTION NOTE
		//This function gets the max bayes prob of "value" in the vector of pdf's "gds"
		//Max Bayes Prob = Max( P(Ci|X) ) = max( P(X|Ci)*P(Ci)/P(X) )
		static double getMaxBayesProb(double value, vector<GaussianDistribution> gds)
		{
			//Find the total no. of points in gds
			int n=0;
			for(unsigned int j=0; j<gds.size(); j++)
				n += gds[j].getN();

			//Find the total probability of aspectRatio : P(X) : evidence
			double totalProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
				totalProb += gds[j].getProbability(value) * (double)gds[j].getN()/n;

			double maxProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
			{
				double currProb1 = gds[j].getProbability(value); //Probability of aspRatio in a given Distribution : P(X|Ci) : likelihood
				double currProb2 = (double)gds[j].getN()/n; //Probability of the distribution among all distributions : P(Ci) : prior
				double currProb = currProb1*currProb2/totalProb; //Probability of this class, given the aspRatio : P(Ci|X) : posterior
				if(currProb>maxProb)
					maxProb=currProb;
			}

			return maxProb;
		}

		//FUNCTION NOTE
		//This function gets the index of pdf, in the vector of pdf's "gds",
			//which gives, max bayes prob for "value"
		//Max Bayes Prob = Max( P(Ci|X) ) = max( P(X|Ci)*P(Ci)/P(X) )
		//Max Bayes Prob Idx = "i" for which "P(Ci|x)" is maximum
		static int getMaxBayesIdx(double value, vector<GaussianDistribution> gds)
		{
			//Find the total no. of points in gds
			int n=0;
			for(unsigned int j=0; j<gds.size(); j++)
				n += gds[j].getN();

			//Find the total probability of aspectRatio : P(X) : evidence
			double totalProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
				totalProb += gds[j].getProbability(value) * (double)gds[j].getN()/n;

			double maxProb = 0;
			int maxProbIdx = 0;
			for(unsigned int j=0; j<gds.size(); j++)
			{
				double currProb1 = gds[j].getProbability(value); //Probability of aspRatio in a given Distribution : P(X|Ci) : likelihood
				double currProb2 = (double)gds[j].getN()/n; //Probability of the distribution among all distributions : P(Ci) : prior
				double currProb = currProb1*currProb2/totalProb; //Probability of this class, given the aspRatio : P(Ci|X) : posterior
				if(currProb>maxProb)
				{
					maxProb=currProb;
					maxProbIdx = j;
				}
			}

			return maxProbIdx;
		}

		//FUNCTION NOTE
		//This function gets the total prob of "value" in the vector of pdf's "gds"
		//It is also called as evidence probability(in Bayes Classifiers)
		//Total Prob P(X) = sum( P(X|Ci)*P(Ci) )
		static double getTotalProb(double value, vector<GaussianDistribution> gds)
		{
			//Find the total no. of points in gds
			int n=0;
			for(unsigned int j=0; j<gds.size(); j++)
				n += gds[j].getN();

			//Find the total probability of aspectRatio : P(X) : evidence
			double totalProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
				totalProb += gds[j].getProbability(value) * (double)gds[j].getN()/n;

			return totalProb;
		}

		//FUNCTION NOTE
		//This function gets the max bayes prob, assuming equal class probabilities
		static double getMaxEqBayesProb(double value, vector<GaussianDistribution> gds)
		{
			//Find the total probability of aspectRatio : P(X) : evidence
			double totalProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
				totalProb += gds[j].getProbability(value);

			double maxProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
			{
				double currProb1 = gds[j].getProbability(value); //Probability of aspRatio in a given Distribution : P(X|Ci) : likelihood
				double currProb = currProb1/totalProb; //Probability of this class, given the aspRatio : P(Ci|X) : posterior
				if(currProb>maxProb)
					maxProb=currProb;
			}

			return maxProb;
		}

		//FUNCTION NOTE
		//This function gets the maximum of the normalized pdf values
		//Normalized pdf value = pdf(given point) / pdf(mean of gaussian)
		static double getMaxNormPdfProb(double value, vector<GaussianDistribution> gds)
		{
			double currProb;
			double maxProb = 0;
			for(unsigned int j=0; j<gds.size(); j++)
			{

				currProb = gds[j].getProbability(value) / gds[j].getProbability(gds[j].getMean());
				if(currProb>maxProb)
				{
					maxProb=currProb;
				}
			}

			return maxProb;
		}

		//FUNCTION NOTE
		//This function gives the index of pdf, in the vector of pdf's "gds",
			//which gives maximum norm pdf value.
		//Normalized pdf value = pdf(given point) / pdf(mean of gaussian)
		static int getMaxNormPdfIdx(double value, vector<GaussianDistribution> gds)
		{
			double currProb;
			double maxProb = 0;
			int maxProbIdx = 0;
			for(unsigned int j=0; j<gds.size(); j++)
			{
				currProb = gds[j].getProbability(value) / gds[j].getProbability(gds[j].getMean());
				if(currProb>maxProb)
				{
					maxProb=currProb;
					maxProbIdx = j;
				}
			}

			return maxProbIdx;
		}

};

#endif //QORQL_STATISTICALRECOGNITION_RECOGNIZERS_RECOGNIZER_HPP_
