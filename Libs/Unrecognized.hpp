#ifndef LIBS_UNRECOGNIZED_H
#define LIBS_UNRECOGNIZED_H


//#include <iostream>
//#include <memory>
//#include <utility>
//#include <vector>
//#include <algorithm>
//#include <string>
//#include <cstdio>
//#include <fstream>
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

#include "../Qorql_headers.h"

//using namespace mongo;
using namespace std;



class Unrecognized
{
	private:
		//Object of Prescription
		Prescription prescription;
		

	public:
		/*
		Constructor to tag and prescription
		*/
		Unrecognized(Prescription prescription)
		{
			this->prescription=prescription;
			
		}
		Unrecognized()
		{
			
		}

		/*

		Display Recognized in file 
		*/
		void Display(string filename="")
		{
			//Displaying in terminal
			if(filename.compare("")==0)
			{
				prescription.show_prescription();
				
			}
			else
			{
				prescription.show_prescription(filename);
				ofstream myfile;
				myfile.open (filename, ios::out | ios::app | ios::binary);
				myfile<<std::endl;
				myfile.close();
				
			}
		}

		void writeToDb(DBClientReplicaSet* conn)
		{
			BSONObj b = BSON("docid" << prescription.Get_DOC() << "pres_url" << prescription.Get_URL() << "tag" << "");
			boost::mutex mut;
			boost::mutex::scoped_lock _lock(mut);
			conn->insert("events.unreco", b);
			_lock.unlock();
			
		}



};


#endif