#ifndef LIBS_RECOGNIZED_H
#define LIBS_RECOGNIZED_H


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
//#include "../Db/Db.h"
//
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



class Recognized
{
	private:
		//Object of Prescription
		Prescription prescription;
		//Recognized tag
		string Rtag;

	public:
		/*
		Constructor to tag and prescription
		*/
		Recognized(Prescription prescription,string tag)
		{
			this->prescription=prescription;
			this->Rtag=tag;
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
				printf("%s\n",Rtag.c_str());
			}
			else
			{
				prescription.show_prescription(filename);
				ofstream myfile;
				myfile.open (filename, ios::out | ios::app);
				myfile<<Rtag<<std::endl;
				myfile.close();
			}
		}

		void writeToDb(DBClientReplicaSet* conn)
		{
			
			BSONObj b = BSON("docid"<< prescription.Get_DOC()<< "pres_url"<< prescription.Get_URL() << "tag" << this->Rtag ) ;
			boost::mutex mut;
			boost::mutex::scoped_lock _lock(mut);
			conn->insert("events.reco", b);
			_lock.unlock();
			cout << "tag:" << this->Rtag << "written to db" << endl;
		
		}


};


#endif