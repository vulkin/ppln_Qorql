#ifndef LIBS_SEMAPHORES_HPP
#define LIBS_SEMAPHORES_HPP

//
//#include <iostream>
//#include <boost/thread.hpp>
//#include <boost/date_time.hpp>
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
//#include <boost/lexical_cast.hpp>
//
//#include <memory>
//#include <utility>
//#include <vector>
//#include <algorithm>
//#include <string>

#include "Qorql_headers.h"

using namespace std;


class semaphore
{
private:
	//Mutex 
	boost::mutex mutex_;

	//Condition variable for checking condition when to lock and unlock
	boost::condition_variable condition_;

	//Count of Prescription that are not fully processed
	unsigned long count_;

public:
	semaphore()
		: count_()
	{}

	//Storing Initial value
	void reset(int x)
	{
		count_ = x;
	}

	//Reducing after every Prescription is fully processed
	void reduce()
	{
		boost::mutex::scoped_lock lock(mutex_);
		count_--;
	}

	//Displaying current value of Count
	void display()
	{
		boost::mutex::scoped_lock lock(mutex_);
		cout << count_ << "!!!!!!" << endl;
	}

	//Notifying when it is complete
	void notify()
	{
		boost::mutex::scoped_lock lock(mutex_);

		if (count_ == 0)
			condition_.notify_one();
	}

	//Wait till notification comes
	void wait()
	{

		boost::mutex::scoped_lock lock(mutex_);
		//cout<<"********************\n";
		while (count_ != 0)
		{
			condition_.wait(lock);

		}

	}
};

#endif