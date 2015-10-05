

#ifndef CONFIG_FILE
#define CONFIG_FILE

/*
This is a full system configuration file. Every section includes the parameters defined in
the corresponding header. Changing the values of these will have a effect on the whole system output.
*/


// Word.hpp configuration
#define INTERPOLATION INTER_LANCZOS4
#define rz_word_row 64			// Defined in System_ver0.2.hpp, under system_learn method.
#define rz_word_col 256			// Defined in System_ver0.2.hpp, under system_learn method.


// filter_bank.hpp configuration
#define FILTER_BCKGND_LINEAR 0
#define FILTER_BCKGND_CONST  1
#define FILTER_BCKGND_UNITY  2
#define rz_rows 1280
#define rz_cols 720
#define MorphOpen_Col_multiplier 9
#define MorphOpen_Row_multiplier 9
#define difference_multiplier 5
#define Filter_threshold 150

// Features.hpp configuration
#define TEMP_MATCH_METHOD CV_TM_CCOEFF_NORMED	// getCorrelation();
#define REDUCTION_METH CV_REDUCE_AVG			// getRowPixelDensity(); && getColPixelDensity();


// Standard Resize flag
#define Standard_resize false	// Standard resize flag, resizes all the images to a standard rz_rows and rz_cols size.
							// This should be the same for training and testing for optimum results.

// word_crop.hpp

#define FILTER_THRESH 130
#define CANNY_LOW_THRESH 40
#define CANNY_HIGH_THRESH 100
#define MORPHO_ELEMENT MORPH_RECT
#define MORPHELE_WIDTH_MULT 9
#define MORPHELE_HEIGHT_MULT 5
#define DILATE_ITER 1
#define GAUSS_WIN_ROW_multiplier 7
#define GAUSS_WIN_COL_multiplier 7
#define GAUSS_VAR 4
#define mser_del 4
#define min_area 1000
#define max_area 30000
#define max_variation 0.25
#define min_diversity 0.9
#define area_threshold 1.01
#define max_evolution 200
#define min_margin 0.003
#define edge_blur_size 5

#define PIX_DEN_THRESH 0.02
#define ASP_RAT_LOW 0.2
#define ASP_RAT_HIGH 1.5


// NN_design configuration
#define transfer_function CvANN_MLP::SIGMOID_SYM
#define TF_alpha 1
#define TF_beta 1
#define NUM_LAYERS 2
#define ITERATIONS 100
#define EPSILON 0.00002f
#define Train_method CvANN_MLP_TrainParams::BACKPROP
#define weight_gradient 0.03f
#define momentum_term 0.001f
#define TRAIN_FLAG CvANN_MLP::NO_INPUT_SCALE
#define IMPLEMENT_MIN_CUTOFF 1
#define RESULT_THRESH 0.7

#endif