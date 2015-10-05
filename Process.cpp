//#include <iostream>
//#include <string>
//#include <cstdlib>
//#include<cstring>
//#include <fstream>
//#include <sstream>
//
//
//#include<boost\asio/io_service.hpp>
//#include<boost/bind.hpp>
//#include<boost/thread/thread.hpp>
//#include<boost/function.hpp>
//#include"semaphore.h"
//#include<opencv2\ml\ml.hpp>
//
//#include "Prescription\Prescription.hpp"
//#include "Task\Threadcall.h"
//#include "Threadpool.h"
//#include "Doc\Doc_block.h"
//
//#include "Db\Db.h"

#include "Qorql_headers.h"

using namespace std;



int NUMBER_OF_DOC = 1;



//All prescription which are to be processed by every doctor in this process
//vector< vector<Prescription> > all_pres;


/*
This function reads all the prescription form every doctor given to this process
READ_PRESCRIPTION(mongo::DBClientReplicaSet * conn,char **argv)
Inputs: DBClientReplicaSet * conn - it is pointter to MongoDb connection
char **argv - List of all doctor id's

I
*/
//void READ_PRESCRIPTION(mongo::DBClientReplicaSet * conn, char **argv)
//{
//	for (int i = 0; i<NUMBER_OF_DOC; i++)
//	{
//		vector<Prescription > vc;
//		
//		string docid(argv[i ]);
//
//		auto_ptr<mongo::DBClientCursor> cursor = conn->query("events.pres",MONGO_QUERY("docid"<< docid) ) ;
//		while (cursor->more())
//		{
//			mongo::BSONObj r = cursor->next();
//			cout << "Processing JSON document: " << r.toString() << std::endl;
//			string url = r.getStringField("url");
//
//			Calling Prescription object
//			Prescription ps(docid, url);
//			cout << ps.Get_URL() << endl;
//			vc.push_back(ps);
//
//		}
//
//		all_pres.push_back(vc);
//	}
//}




// alternate method to load prescription from disk
void loadPrescriptions(vector<Prescription>* pres,int pres_count)
{
	for (int i = 1; i <= pres_count; i++)
	{
		string url = "1.jpg";
		string urlindex = to_string(i) + "jpg";
		cout << urlindex << endl;
		string docid = "ajay ";
		Prescription ps(docid, url);

		pres->push_back(ps);



}

}


int main()
{
#ifdef _TIME_CHECK_

	time_t start_time;
	time(&start_time);
#endif

//	Checking if Doctor id's are given
//	if (argc<2)
//	{
//		printf("No doctor given to this process...\n");
//		return 1;
//	}
//
//	NUMBER_OF_DOC = argc - 1;


	//// initialising Database connection
	//Db db;
	//db.clientInit();
	//db.connect(default_uri);

	

	char* doc_ids[] = { "ajay" };
	cout << doc_ids[0] << endl;

	
	//reading prescription of all doctors
	//READ_PRESCRIPTION(db.conn,doc_ids);

	
		int threadcount ;
		int pres_count ;

		cout << "Enter the number of prescriptions:" << endl;
		cin >> pres_count;
		cout << endl;

		cout << "enter the number of threads:" << endl;
		cin >> threadcount;
		cout << endl;


#ifdef _TESTING_MODE_
		//Total number of Doctor Assigned
		printf("Total number of doctor assigned to this process =%d\n", NUMBER_OF_DOC);
#endif


		//Process Initialization----------------------

		// reading the number of cores available on cpu(including hyperthreading)
		unsigned int ncore = boost::thread::hardware_concurrency();
		cout << "number of cores: " << ncore << endl;

		//Calling  Threadpool objects
		bamthread::ThreadPool pool(threadcount);

#ifdef _TESTING_MODE_
		printf("%d Threads created successfully...\n", NUM_OF_THREADS);
#endif

//
//		int total_prescription_count = 0;
//
//		for (int i = 0; i<NUMBER_OF_DOC; i++)
//		{
//			for (int j = 0; j<vec_prescription[i].size(); j++)
//			{
//				total_prescription_count++;
//				vec_prescription[i][j].show_prescription();
//			}
//		}

		semaphore*  m_semaphore = new semaphore();

		//to read nn object from xml file
		const char* xmlfilename = "mlp_save.xml";
				

		//lookup table file written in this csv from disk
		ifstream csvfile("temp_train.csv");

		//Calling each doctor one by one 
		for (int i = 0; i<NUMBER_OF_DOC; i++)
		{

#ifdef _TIME_CHECK_

			time_t doc_start_time;
			time(&doc_start_time);
#endif

			string docid("docnum:" + to_string(i + 1));
			cout << "docid:" << i + 1 << endl;

			vector<Prescription> pres;
			loadPrescriptions(&pres, pres_count);
			//pres = all_pres[i];

			// creating doctor object 
			DocBlockTesting doc(docid, pres.size(), pres, &pool, m_semaphore);

			cout << "doc object created" << endl;

			//reading xml NN file and lookup table
			doc.loadDoctorVocab(xmlfilename, &csvfile);

			time_t startTime;
			time(&startTime);

			//posting prescriptions to the threads in this function
			doc.MAIN_WORK();

			time_t endTime;
			time(&endTime);

			cout << format("\nTotal time %f", difftime(endTime, startTime));
			
			cout << "next doc" << endl;

#ifdef _TIME_CHECK_

			time_t doc_end_time;
			time(&doc_end_time);
			cout << "\n-----------------------------------------------------------";
			cout << format("\nTotal time of %dth Doctor with %d Prescription is %f\n", i + 1, vec_prescription[i].size(), difftime(doc_end_time, doc_start_time));
#endif
		}


#ifdef _TIME_CHECK_

		time_t end_time;
		time(&end_time);
		cout << "\n-----------------------------------------------------------";
		cout << format("\nTotal time of %d Doctor with %d Prescription is %f\n", argc - 1, total_prescription_count, difftime(end_time, start_time));
#endif




		delete m_semaphore;



		cout << "main end" << endl;


	

	return 0;



}


