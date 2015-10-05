#ifndef DOC_BLOCK_H
#define DOC_BLOCK_H

//#include <iostream>
//#include <memory>
//#include <utility>
//#include <vector>
//#include <algorithm>
//#include <string>
//#include <cstdio>
//
//
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
//
////ThreadPool libs
//#include "../Threadpool.h"
//
//
////Semaphore library
//#include "../semaphore.h"
//
////ThreadCall libs
//#include "../Task/Threadcall.h"
//
////Neural Network class
//#include "../IP_block/Neural_Network/NN_design.hpp"


#include "../Qorql_headers.h"


using namespace std;
//using namespace mongo;
using namespace cv;



class DocBlockTesting
{
private:

	//Total count of Prescription for that doctor
	int Prescription_count;
	//Doctor ID
	string Doc_ID;
	
	//prescriptions for this doctor
	vector<Prescription> pres_it;


	//Pointer to threadPool 
	bamthread::ThreadPool *ptr_pool;

	//Database Connection ptr
	//mongo::DBClientReplicaSet * connection;

	//Flag for checking if all precription is tested
	int flag;
	
	bool gRunning;

	//semaphore
	semaphore *m_semaphore ;

	//vocab per doctor
	map<int, string> LookupTable;
	nn_design NNobj;

public:
	/*typedef boost::shared_mutex Lock;
	typedef boost::shared_lock< Lock > ReadLock;
	// typedef boost::unique_lock< Lock > WriteLock;
	Lock myLock;

	std::condition_variable cv;
	boost::mutex gWorkQueueMutex;*/


	/*
	Constructor to get Doctor id,Total count of prescription,Pointer to thread pool
	Pointer to vector of Prescription
	*/
	DocBlockTesting(string id, int Pcount, vector<Prescription> itr,
		bamthread::ThreadPool *poolptr, semaphore *m_semaphore)
	{
		Doc_ID = id;
		Prescription_count = Pcount;
		pres_it = itr;
		ptr_pool = poolptr;
		flag = 0;
		
		gRunning = true;
		this->m_semaphore = m_semaphore;
		//this->connection = conn;
	}


	void Thread_function(int i, semaphore *m_semaphor, Prescription itrP, map<int, string> LTable,
		 int siz)
	{		
		
		cout << "task:" << i << "started" << endl;

		ThreadCall for_every_Prescription(i, &LTable, &itrP, LTable.size(), &NNobj);
		for_every_Prescription.READ_URL();
		for_every_Prescription.IP_BLOCK(i);

		m_semaphor->reduce();
		//m_semaphor->display();
		m_semaphor->notify();

		cout << "task:" << i << "terminating" << endl;
	}


	/*Method  INIT()
	Will invoke vocabulary
	*/
	//void INIT()
	//{
	//	string url;
	//	//Database
	//	auto_ptr<mongo::DBClientCursor> cursor = conn->query("events.Vocab", QUERY("DOCTORID" << atoi(Doc_ID.c_str())));
	//	while (cursor->more())
	//	{
	//		BSONObj r = cursor->next();
	//		//cout << "Processing JSON document: " << r.toString() << std::endl;
	//		url = r.getStringField("URL");
	//		std::vector<BSONElement> be = r.getField("VOCAB_TABLE").Array();
	//		for (unsigned int i = 0; i<be.size(); i++)
	//		{

	//			BSONObj bo = be[i].embeddedObject();
	//			string s1 = bo.getStringField("Class");
	//			string s2 = bo.getStringField("Label");
	//			int kk = atoi(s2.c_str());
	//			LookTable[kk] = s1;
	//			// cout<<kk<<" "<<s1<<endl;
	//		}
	//		break;
	//	}


	//	//Storing NNURL
	//	NNobj.StoreURL(url);

	//	//printf("Doctor %s vocab is loaded\n",Doc_ID.c_str() );

	//}

	void loadLookupTable( ifstream* file)
	{
		string line;



		while (getline(*file, line))
		{
			istringstream linestream{ line };

			vector<string> tokens;
			string token;

			while (getline(linestream, token, ';'))
			{
				tokens.push_back(token);

			}

			//cout << line << endl;
			//cout << tokens[0] << tokens[1] << endl;

			int label = atoi(tokens[1].c_str());
			this->LookupTable.insert(make_pair(label, tokens[0]));

		}

	}

	void loadDoctorVocab( const char* filename,   ifstream* file)
	{
		//reading xml file
		//this->NNobj.load(filename, name);

		this->NNobj.neural_net.load(filename);

		cout << "NN obj loaded from xml file" << endl;

		cout << "loading lookup table from csv file..." << endl;
		loadLookupTable(file);

	}



	/*
	Method  MAIN_WORK()
	will call ThreadCall Class
	for every prescription
	*/
	void MAIN_WORK()
	{
		//vector<Prescription>::iterator itrP=pres_it;
		//calling Thread_block
		m_semaphore->reset(Prescription_count);

		cout << Prescription_count << endl;

		for (int i = 0; i<Prescription_count; i++)
		{

			ptr_pool->service.post(boost::bind(&DocBlockTesting::Thread_function, this, i, m_semaphore, pres_it[i], LookupTable,  LookupTable.size()));
			cout << "post:" << i << endl;
		}

		m_semaphore->wait();
		
	}

	/*Method Exit()

	It will return confirmation if all prescription
	is written properly
	*/
	void EXIT()
	{

	}

};


#endif