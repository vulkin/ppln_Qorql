#ifndef THREADCALL_THREADCALL_H
#define THREADCALL_THREADCALL_H


//#include <iostream>
//#include <memory>
//#include <utility>
//#include <vector>
//#include <algorithm>
//#include <string>
//#include <cstdio>
//
//
////mongo db classes
////#include <mongo/client/dbclient.h> 
////#include <mongo/client/init.h>// the mongo c++ driver
//
//
////Boost libraries for multi threading 
//#include <boost/thread.hpp>
//#include <boost/date_time.hpp>
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/thread/locks.hpp>
//#include <boost/thread/shared_mutex.hpp>
//
////OpenCV library
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/features2d/features2d.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/ml/ml.hpp>
//
////Prescription lib
//#include "../Prescription/Prescription.hpp"
//
////Filters
//#include "../IP_block/Filtering/filter_bank.hpp"
//
////Cropping 
//#include "../IP_Block/Scissoring/Cropping/Cropping.hpp"
//
////Scissoring 
//#include "../IP_block/Scissoring/word_crop.hpp"
//
////Straightening 
//#include "../IP_Block/Scissoring/Straightening/Straightening.hpp"
//
//
////Word
//#include "../IP_Block/Word.hpp"
//
////Recognized
//#include "../Libs/Recognized.hpp"
//
////Unrecognized
//#include "../Libs/Unrecognized.hpp"
//
//#include "../IP_block/Neural_Network/NN_design.hpp"

#include "../Qorql_headers.h"


//#define _TESTING_MODE_THREAD_BLOCK_
//#define _TIME_CHECK_IP_

#define BUG std::cout<<"BUG\n"<<std::endl;
#define RESIZE_COLUMN 256
#define RESIZE_ROW 64


//using namespace mongo;

using namespace std;
using namespace cv;


class ThreadCall
{
private:
	//Pointer to vocabulary
	map<int, string>* LookTablePtr;

	//neural network wrapper class
	nn_design* NNobj;

	int LookTablePtrSize;

	//Image of that prescription
	Mat img_prescription;

	//Object of prescription
	Prescription *Pprescription;
	
	//vector<Recognized>
	vector<Recognized> recognized_vec;

	//vector<Unrecognized>
	vector<Unrecognized> unrecognized_vec;
	
	//database connection pointer
	//DBClientReplicaSet * conn;

	int val;

public:
	
	/*
	Constructor to vocabulary details and prescription
	*/
	ThreadCall(int i, map<int, string>* LookTablePtr, Prescription *prescription, int LookTablePtrSize, nn_design* NNobj)
	{
		this->LookTablePtr = LookTablePtr;
		this->NNobj = NNobj;
		this->Pprescription = prescription;
		//this->conn = conn;
		val = i;
		this->LookTablePtrSize = LookTablePtrSize;
		//this->conn = conn;
		//cout<<"Conn loaded\n";
	}

	/*

	Method to read image from given URL
	*/
	void READ_URL()
	{

		//cout<<NNobjPtr->get_layer_count()<<endl;

		string URL_pre = Pprescription->Get_URL();
		//cout<<URL_pre<<endl;

		img_prescription = cv::imread(URL_pre, 0);


#ifdef _TESTING_MODE_THREAD_BLOCK_

		imwrite(format("Before Filter %d.png", val), img_prescription);
		Pprescription->show_prescription();
#endif	


	}


	/*
	Method to store compressed version of that image to specific location
	*/
	void Store_Compressed()
	{

	}

// HOG feature extraction of every scissored word 
	Mat extractfeatures(Vector<Word> wrd_cropped)
	{
		vector<vector<double>> tmp_feat;


		for (int i = 0; i < wrd_cropped.size(); i++)
		{

			wrd_cropped[i].resizeImage(Size(rz_word_col, rz_word_row));
			tmp_feat.push_back(Features::getHOG(wrd_cropped[i].getImage()));
		}

		Mat feat_vects = Mat::zeros(tmp_feat.size(), tmp_feat.at(0).size(), CV_64F);

		for (int i = 0; i < tmp_feat.size(); i++)
		{
			for (int j = 0; j < tmp_feat.at(i).size(); j++)
				feat_vects.at<double>(i, j) = tmp_feat.at(i).at(j);
		}

		return feat_vects;

	}
	
	
	/*
	Method containing Image processing block
	*/
	void IP_BLOCK(int i)
	{

		cout << "Entering in IP block:" << "task:" << i;

		//checking if image is empty or not
		if (!img_prescription.data)
		{
			cerr << "No Image data found. Aborting execution..." << endl;
			abort();
		}
		else
		{ cout << "checked image data:not empty:task:" << i << endl; }

		// converting to grayscale

		cout << "converting to gray scale:task:" << i << endl;
		Mat img_gray;
		if (img_prescription.channels() > 1)
			cvtColor(img_prescription, img_gray, COLOR_BGR2GRAY);
		else
			img_prescription.copyTo(img_gray);
		cout << "converted:task:" <<i<< endl;

		//if standar =d size is required
		if (Standard_resize)
		{
			cout << "standard resizing:task:" << i << endl;
			resize(img_gray, img_gray, Size(rz_cols, rz_rows), 0, 0, INTERPOLATION);
			cout << "resized:task:" <<i<< endl;
		}

#ifdef _TIME_CHECK_IP_
		time_t IP_start_time;
		time(&IP_start_time);
#endif

		//Filtering 
		filter_class filters;
		Mat img_clean;
		cout << "filtering:start:task:" << i << endl;
		
		time_t filt_start_time;
		time(&filt_start_time);

		img_clean = filters.filter_gautam(img_gray,i);

		time_t filt_end_time;
		time(&filt_end_time);
		cout << format("filtering time: %f", difftime(filt_end_time, filt_start_time)) << "task:" << i << endl;

		cout << "filtering:end:task:" << i << endl;



#ifdef _TIME_CHECK_IP_
		time_t filter_time;
		time(&filter_time);
		cout << "\n-----------------------------------------------------------";
		cout << format("\nTotal time for filter is %f", difftime(filter_time, IP_start_time));
#endif


#ifdef _TESTING_MODE_THREAD_BLOCK_

		imwrite(format("ss%d.png", val), img_prescription);
		Pprescription->show_prescription();
		//
#endif	

		//Scissoring 
		word_crop v_scissor;
		cout << "scissoring:start:task:" << i << endl;

		vector <Word> wrd = v_scissor.scissor_scale(img_prescription, i);

		cout << "scissoring:end:task:" << i << endl;

		if (wrd.size() == 0)
		{
			cout << "Word cropping yields no images. Continuing with execution.." << endl;
			//return vector<Word>();
	}


#ifdef _TIME_CHECK_IP_
		time_t scissor_time;
		time(&scissor_time);
		cout << "\n-----------------------------------------------------------";
		cout << format("\nTotal time for scissor is %f", difftime(scissor_time, filter_time));
#endif

		//Cropping and Straightening all image
		cout << "cropping and straightening started:task:" << i << endl;
		for (int i = 0; i<wrd.size(); i++)
		{

			//Print
#ifdef _TESTING_MODE_THREAD_BLOCK_
			imwrite(format("After Scissored%d_%d.png", val, i), wrd[i].getImage());
			Pprescription->show_prescription();
			cout << 2 << endl;
#endif	


			//Cropping 
			Cropping crp;
			Mat input_img = wrd[i].getImage();
			Mat output_img = crp.crop(input_img);

			if (!output_img.empty())
			{
				wrd[i].setImage(output_img);
			}
			
			
#ifdef _TIME_CHECK_IP_
			time_t crop_time;
			time(&crop_time);
			cout << "\n-----------------------------------------------------------";
			cout << format("\nTotal time for crop is %f", difftime(crop_time, filter_time));
#endif
			}

		cout << "crop and strt end:task:" << i << endl;


		//Feature Extraction
		Mat feat_mat;
		feat_mat = extractfeatures(wrd);
				

		cout << "nn block start:task:" << i << endl;

		vector <int> results ;

		if (NNobj->neural_net.get_layer_count() == 0)
			{

				cout << "untrained neural network...setting all to unrecognised" << endl;

				for (int i = 0; i < wrd.size(); i++)
				{
					results.push_back(-1);
				}
			}


		else
		{
			results = NNobj->testNN(feat_mat,LookTablePtrSize,i);

		}

		cout << "layer count after testNN returns:" << NNobj->neural_net.get_layer_count()<<"," <<"task:"<<i<< endl;

		if (wrd.size() != results.size())
		{
			cout << "Smthng is wrng..size does not match!!!" << endl;
			abort();
		}

		for (int i = 0; i < results.size(); i++)
		{
			//cout << results.at(i) << endl;
			if (results.at(i) > -1)
				wrd[i].appendTag((*LookTablePtr)[results.at(i)]); // Tagging the word fom vector<Word> objects.
			else
				wrd[i].appendTag("-1");
			//imshow("googoo", img_cropped_words.at(i).getImage());
			//waitKey(0);
		}

		cout << "Done nn block:task" << i << endl;

#ifdef _TIME_CHECK_IP_
		time_t NN_time;
		time(&NN_time);
		cout << "\n-----------------------------------------------------------";
		cout << format("\nTotal time for NN is %f", difftime(NN_time, post_processing_time));
#endif

		string recog = "Saved_Results/Recognized_word.txt", unrecog = "Saved_Results/Unrecognized_word.txt";
		for (int i = 0; i<wrd.size(); i++)
		{
			if (wrd[i].getTag().compare("-1") != 0)
			{
				Recognized reco(*Pprescription, wrd[i].getTag());
				//cout<<wrd[i].getTag()<<endl;
				cout << "writing recognized tag to file" << "task:" << val << endl;
				string wrd_url ="wrd_" + to_string(i) + "tag_"+ wrd[i].getTag() +".jpg";
				imwrite("Saved_Results/"+ wrd_url, wrd[i].getImage());
				reco.Display(recog);
				//reco.writeToDb(conn);
				
			}
			else
			{

				Unrecognized unreco(*Pprescription);
				cout << "unrecognized" << "task:" << val << endl;
				string wrd_url = "wrd_" + to_string(i) + "_unreco" + ".jpg";
				imwrite("Saved_Results/" + wrd_url , wrd[i].getImage());
				unreco.Display(unrecog);
				//unreco.writeToDb(conn);
			}
		}

	}


	
	



};

#endif
