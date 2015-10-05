#ifndef THREADPOOL_H
#define THREADPOOL_H

//
//#include<iostream>
//
//#include<boost\asio/io_service.hpp>
//#include<boost/bind.hpp>
//#include<boost/thread/thread.hpp>
//#include<boost/function.hpp>

#include "Qorql_headers.h"
#include "Db\Db.h"
using namespace std;

namespace bamthread

{
	typedef unique_ptr<boost::asio::io_service::work> asio_worker;


	class ThreadPool
	{
	public:

		boost::asio::io_service service; //io service..it's run method invokes the work in the thread
		boost::thread_group group;  // threads will be added to thi thread group.
		asio_worker working;  // needed so that service threads don't terminate before completing tasks.
		

		ThreadPool(size_t threadcount) : service(), working(new asio_worker::element_type(service))
		{
			for (size_t i = 0; i < threadcount; i++)
			{				
				auto workerthread = boost::bind(&boost::asio::io_service::run, &(this->service));
				group.add_thread(new boost::thread(workerthread));
			}
		}

		void thread_func(boost::asio::io_service service, DBClientReplicaSet* client)
		{
			DBClientReplicaSet* connection = client;
			service.run();

		}

		~ThreadPool()
		{
			working.reset();
			group.join_all();
			service.stop();

		}




	};

}
#endif
