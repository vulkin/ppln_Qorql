// Neural Network class.

#ifndef NN_DESIGN_HPP
#define NN_DESIGN_HPP


#include "../qorql_IP.hpp"


class nn_design
{
public:

	// Class variables
	CvANN_MLP neural_net;
	int inputs, outputs;
	Mat layer_map;


	// Constructors

	nn_design(){};
	void createNN(int, int, int = 0);
	void trainNN(Mat, Mat, bool = false, bool = false);
	//Mat testNN(Mat);
	vector<int> result(Mat, bool = IMPLEMENT_MIN_CUTOFF, double = RESULT_THRESH);
	vector<int> testNN(Mat);
	vector<int> testNN(Mat, int,int);
	
	
};


// Class Definitions

void nn_design::createNN(int input_nodes, int output_nodes, int num_layers)
{

	if (input_nodes <= 0)
	{
		cerr << "Input nodes should be a positive integer." << endl;
		abort();
	}

	if (output_nodes <= 0)
	{
		cerr << "Input nodes should be a positive integer." << endl;
		abort();
	}

	inputs = input_nodes;		// For check in testNN() method
	outputs = output_nodes;		// For check in testNN() method
	
	(num_layers < 1)? num_layers = 1: NULL;

	num_layers = num_layers + 2;
	Mat layer = Mat(num_layers, 1, CV_32S);
	int tmp_num_layer_node = input_nodes;

	// This is loop which assigns the number of neurons in successive layers in decreasing order.
	layer.row(0) = Scalar(tmp_num_layer_node);
	
	if (tmp_num_layer_node > 10)  // If the input vector is large, successive layers decrement with a factor of 2/3 approx.
	{
		//layer.row(1) = Scalar(450);
		//layer.row(2) = Scalar(50);
		for (int i = 1; i < num_layers - 1; i++)
		{
			tmp_num_layer_node = (0.6 * tmp_num_layer_node);

			if (tmp_num_layer_node < output_nodes)
				tmp_num_layer_node = output_nodes + 1;
			layer.row(i) = Scalar(tmp_num_layer_node);
		}
	}
	else						 // If the input vector is small, successive layers decrement by unity.
	{
		if (num_layers > 8)		// Input is small and layers are more.
		{
			cout << "Layer assignment failed.!!" << endl;
			cout << "Decrease layers or modify input." << endl;
			abort();
		}

		for (int i = 1; i < num_layers - 1; i++)
		{
			tmp_num_layer_node--;
			if (tmp_num_layer_node > 2)
				layer.row(i) = tmp_num_layer_node;
			else
				layer.row(i) = 2;
		}
	}

	if (num_layers > 3)	// If more than one hidden layers (excluding input and output layers), the following is for last hidden layer check
	{
		if (tmp_num_layer_node > 5 * output_nodes)
			layer.row(num_layers - 2) = Scalar(2 * output_nodes);
		else
			layer.at<int>(num_layers - 2, 0) = 0.6 * tmp_num_layer_node;
	}

	layer.row(num_layers - 1) = Scalar(output_nodes);
	layer.copyTo(layer_map);

	neural_net.create(layer, transfer_function, TF_alpha, TF_beta);

	/*if (true)
	{
		cout << "\n\n" << endl;
		cout << "Neural Network Design Overview..." << endl;
		cout << "1) Number of Layers: " << neural_net.get_layer_count() << endl;
		cout << "2) Layer Mapping: ";
		for (int i = 0; i < neural_net.get_layer_count(); i++)
			cout << '\t' << layer.at<int>(i, 0);
		cout << endl;
	}*/
}


void nn_design::trainNN(Mat input, Mat target, bool info, bool timer_flag)
{
	if (!input.data)
	{
		cerr << "No input data to train. Terminating execution..." << endl;
		exit(0);
	}

	if (!target.data)
	{
		cerr << "No target data to train. Terminating execution..." << endl;
		exit(0);
	}

	if (input.rows != target.rows)
	{
		cerr << "Sample mismatch between Inputs and Targets. Terminating execution..." << endl;
		exit(0);
	}
	
	CvTermCriteria criteria;
	criteria.max_iter = ITERATIONS;
	criteria.epsilon = EPSILON;
	criteria.type = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS;

	CvANN_MLP_TrainParams neural_params;
	neural_params.train_method = Train_method;
	neural_params.bp_dw_scale = weight_gradient;
	neural_params.bp_moment_scale = momentum_term;
	neural_params.term_crit = criteria;

	int attributes = input.cols;
	int classes = target.cols;
	nn_design::createNN(attributes, classes, (int) NUM_LAYERS);

	

	if (info)
	{
		cout << "\n\n" << endl;
		cout << "Neural Network Design Overview..." << endl;
		cout << "1) Number of Layers: " << neural_net.get_layer_count() << endl;
		cout << "2) Layer Mapping: ";
		for (int i = 0; i < neural_net.get_layer_count(); i++)
			cout << '\t' << layer_map.at<int>(i, 0);
		cout << endl;

		if (Train_method == 0)
			cout << "3) Training Method: Back-Propagation." << endl;
		else if (Train_method == 1)
			cout << "3) Training Method: Resilient Back-Propagation." << endl;

		cout << "4) Number of Iterations: " << ITERATIONS << endl;
		cout << "5) Weight Gradient: " << weight_gradient << endl;
		cout << "6) Momentum Term: " << momentum_term << endl;

		cout << "\n\n" << endl;
	}

	clock_t timer;
	timer = clock();

	cout << "Processing/Training the Network... Please be patient..." << endl;
	neural_net.train(input, target, Mat(), Mat(), neural_params, TRAIN_FLAG);

	if (timer_flag)
		cout << "Neural Network Training time: " << (float)(clock() - timer) / CLOCKS_PER_SEC << " seconds \n\n" << endl;

}


//Mat nn_design::testNN(Mat test_input)
//{
//	if (!test_input.data)
//	{
//		cerr << "No input data present. Terminating application..." << endl;
//		exit(0);
//	}
//
//	if (neural_net.get_layer_count() == 0)
//	{
//		cerr << "Untrained neural network for the specified instance. Terminating execution..." << endl;
//		exit(0);
//	}
//
//	if (test_input.cols != inputs)
//	{
//		cerr << "Input size doesn't match with the trained NN input size. Terminating execution..." << endl;
//		exit(0);
//	}
//
//	Mat res;
//	res.zeros(Size(test_input.rows, outputs), CV_64F);
//
//	neural_net.predict(test_input, res);
//
//	/*if (info)
//	{
//		for (int i = 0; i < res.rows; i++)
//		{
//			cout << "Sample " << i << " prediction: " << res.row(i) << endl;
//			cout << "\n\n" << endl;
//		}
//	}*/
//
//
//	return res;
//
//}





 vector<int> nn_design::testNN(Mat test_input)
{
	if (!test_input.data)
	{
		cerr << "No input data present. Terminating application..." << endl;
		exit(0);
	}

	if (neural_net.get_layer_count() == 0)
	{
		cerr << "Untrained neural network for the specified instance. Terminating execution..." << endl;
		exit(0);
	}

	/*if (test_input.cols != inputs)
	{
		cerr << "Input size doesn't match with the trained NN input size. Terminating execution..." << endl;
		exit(0);
	}*/

	Mat res;
	res.ones(Size(test_input.rows, outputs), CV_64F);
	res = -1 * res;

	neural_net.predict(test_input, res);

	/*for (int i = 0; i < res.rows; i++)
		cout << "Result: " << res.row(i) << "\n\n" << endl;*/


	vector<int> res_index;
	res_index = result(res, IMPLEMENT_MIN_CUTOFF);

	/*cout << "\n\n" << endl;
	for (int i = 0; i < res_index.size(); i++)
		cout << res_index[i] << '\t';
	cout << "\n\n" << endl;*/

	return res_index;

}

 vector<int> nn_design::testNN(Mat test_input, int LookupTableSize,int tasknum)
 {
	 if (!test_input.data)
	 {
		 cerr << "No input data present. Terminating application..." << endl;
		 exit(0);
	 }

	 if (neural_net.get_layer_count() == 0)
	 {
		 cerr << "Untrained neural network for the specified instance. Terminating execution..." << endl;
		 exit(0);
	 }

	 /*if (test_input.cols != inputs)
	 {
	 cerr << "Input size doesn't match with the trained NN input size. Terminating execution..." << endl;
	 exit(0);
	 }*/

	 Mat res;
	 res.ones(Size(test_input.rows, LookupTableSize), CV_64F);
	 res = -1 * res;

	 neural_net.predict(test_input, res);

	 /*for (int i = 0; i < res.rows; i++)
	 cout << "Result: " << res.row(i) << "\n\n" << endl;*/


	 vector<int> res_index;
	 res_index = result(res, IMPLEMENT_MIN_CUTOFF);

	 /*cout << "\n\n" << endl;
	 for (int i = 0; i < res_index.size(); i++)
	 cout << res_index[i] << '\t';
	 cout << "\n\n" << endl;*/

	 cout << "layer count before testNN returns:" << neural_net.get_layer_count() << "task:" << tasknum << endl;

	 return res_index;

 }


vector<int> nn_design::result(Mat inp, bool cutoff, double th)
{
	//Mat inp = inp_mat + 2;
	if (!inp.data)
	{
		cout << "Invalid data output from Neural Network." << endl;
		abort();
	}

	vector<int> res_index;
	double max = -2;

	for (int k = 0; k < inp.rows; k++)
	{
		int wrd;
		for (int i = 0; i < inp.cols; i++)
		{
			if (inp.at<double>(k, i) > max)
			{
				max = inp.at<double>(k, i);
				wrd = i;
			}
		}
		
		if (max == 1)
			res_index.push_back(-1);
		else
			res_index.push_back(wrd);

		//cout << "Max n Ind: " << max << '\t' << wrd << endl;
		max = -2;
		
	}

	//cout << res_index << endl;

	if (cutoff)
	{
		cout << "\n\n\n" << "Implementing minimum cut-off of " << th  << "\n\n" << endl;
		for (int i = 0; i < res_index.size(); i++)
		{
			//cout << res_index.at(i) << endl;
			//cout << inp.at<double>(i, res_index.at(i)) << endl;
			if ((res_index.at(i) > 0) && (inp.at<double>(i, res_index.at(i)) < th))
				res_index.at(i) = -1;
		}
	}

	return res_index;
}

#endif