#ifndef QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_SOFTTAG_HPP_
#define QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_SOFTTAG_HPP_

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

class SoftTag
{
	private:
		String tag;
		double prob;
		int size;

	public:
		SoftTag()
		{
			tag="";
			prob=0;
			size=0;
		}

		SoftTag(String tag, double prob, int size)
		{
			this->tag = tag;
			this-> prob = prob;
			this->size = size;
		}

		//Getters
		String getTag(){return tag;}
		double getProb(){return prob;}
		int getSize(){return size;}
};

#endif //QORQL_STATISTICALRECOGNITION_CONSOLIDATOR_SOFTTAG_HPP_
