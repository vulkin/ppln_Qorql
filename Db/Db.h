#ifndef DB_H
#define DB_H


#define STATIC_LIBMONGOCLIENT



#include<iostream>
#include<string>
#include <mongo/client/dbclient.h>


#include "../Qorql_headers.h"

using namespace std;
using namespace mongo;

const std::string default_uri = "mongodb://localhost:27017";

class Db
{

	

public:

	DBClientReplicaSet* conn;
	
	

	void clientInit()
	{
		Status status = client::initialize();
		if (!status.isOK())
		{
			std::cout << "failed to initialize the client driver: " << status.toString() << endl;
			
		}
	}

	void connect(string uri)
	{
		//parsing connection string
		string errmsg;
		ConnectionString cs = ConnectionString::parse(uri, errmsg);
		if (!cs.isValid())
		{
			std::cout << "error parsing url: " << errmsg << std::endl;
			return  ;
		}

		//making Connection
		 conn = static_cast<DBClientReplicaSet *>(cs.connect(errmsg, 0));

		if (!conn)
		{
			std::cout << "error connecting: " << errmsg << endl;
			return ;
		}

		cout << "connection established successfully" << endl;
		

	}

};

#endif