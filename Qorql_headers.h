#ifndef QORQL_HEADERS
#define QORQL_HEADERS



#include <iostream>
#include <string>
#include <cstdlib>
#include<cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include<string>


//boost
#include <boost/asio.hpp>
#include<boost\asio/io_service.hpp>
#include<boost/bind.hpp>
#include<boost/thread/thread.hpp>
#include<boost/function.hpp>


//open cv
#include<opencv2\ml\ml.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include "Threadpool.h"

// databse
#include "Db\Db.h"


//Ip block
#include "IP_block\qorql_IP.hpp"




#include "semaphore.h"
#include "Prescription\Prescription.hpp"


// reco & unreco libs
#include "Libs\Recognized.hpp"
#include "Libs\Unrecognized.hpp"


//others

#include "Task\Threadcall.h"
#include "Doc\Doc_block.h"








#endif