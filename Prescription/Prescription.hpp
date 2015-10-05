#ifndef PRESCRIPTION_PRESCRIPTION_H
#define PRESCRIPTION_PRESCRIPTION_H


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
//
//
////Boost libraries for multi threading 
//#include <boost/thread.hpp>
//#include <boost/date_time.hpp>
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
//#include <boost/lexical_cast.hpp>

#include "../Qorql_headers.h"

//using namespace mongo;
using namespace std;



class Prescription
{
	private:
		//Prescription unique id
		string DOCID;
		//Image URL of AWS
		string P_URL;

	public:
		/*
		Constructor to get prescriptions 
		details
		*/
		Prescription(string DOCID,string url)
		{
			this->DOCID=DOCID;
			P_URL=url;
		}

		Prescription()
		{
		
		}

		/*
		Method to Show prescription details 
		*/
		void show_prescription(string filename="")
		{
			if(filename.compare("")==0)
			{
				printf("DOCID:%s - URL:%s\n",DOCID.c_str(),P_URL.c_str());
			}
			else
			{
				ofstream myfile;
				myfile.open (filename, ios::out | ios::app );
				myfile<<DOCID<<" "<<P_URL<<" ";
				myfile.close();
			}
		}

		/*
		Methods to getDetails  of every member of Prescription
		*/
		string Get_URL(void)
		{
			return P_URL;
		}

		string Get_DOC(void)
		{
			return DOCID;
		}


};


#endif