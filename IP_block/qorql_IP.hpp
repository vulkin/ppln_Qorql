
// Header which includes all the libraries and other headers realted to Qorql.
// Including this header will include all other related headers.
// Update this header whenever new module gets added.

#ifndef QORQL_ALL_HEAD
#define QORQL_ALL_HEAD

#include <windows.h>


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <opencv2/opencv.hpp>
#include "config_file.hpp"
#include "Word.hpp"
#include "Filtering\filter_bank.hpp"
#include "Features\Features.hpp"
#include "Scissoring\word_crop.hpp"
#include "Scissoring\Cropping\Cropping.hpp"
#include "Scissoring\Straightening\RansacLineFit.hpp"
#include "Scissoring\Straightening\Straightening.hpp"
#include "Neural_Network\NN_design.hpp"


#endif