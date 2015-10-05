
// Scissoring Version 0.3.
// Filter included.

#ifndef FILTER_SCISSOR
#define FILTER_SCISSOR


#include "../qorql_IP.hpp"


using namespace std;
using namespace cv;


class word_crop
{
public:

	word_crop::word_crop() {};
	static vector<Word> scissor(Mat,int, bool);
	static vector<Word> scissor_scale(Mat,int, bool, bool);

};



// Static Class Definitions
vector<Word> word_crop::scissor(Mat img_inp_rgb,int tasknum=100, bool show_fig = false)
{

	vector<Word> img_words;

	Mat img_inp;
	if (img_inp_rgb.channels() > 1)
		cvtColor(img_inp_rgb, img_inp, CV_BGR2GRAY);
	else
		img_inp_rgb.copyTo(img_inp);

	//cout << img_inp.size() << endl;

	Mat img_edge;
	Canny(img_inp, img_edge, CANNY_LOW_THRESH, CANNY_HIGH_THRESH);
	Mat ele = getStructuringElement(MORPHO_ELEMENT, Size(MORPHELE_WIDTH_MULT, MORPHELE_HEIGHT_MULT), Point(-1, -1));
	dilate(img_edge, img_edge, ele, Point(-1, -1), DILATE_ITER);
	GaussianBlur(img_edge, img_edge, Size(GAUSS_WIN_ROW_multiplier, GAUSS_WIN_COL_multiplier), GAUSS_VAR, GAUSS_VAR);

	/*int delta = 0.05, min_area = 1000, max_area = 30000;
	double _max_variation = 0.25;  double _min_diversity = .9;
	int _max_evolution = 200; double _area_threshold = 1.01;
	double _min_margin = 0.003; int _edge_blur_size = 5;*/

	cout << "scissoring:contours:task:" << tasknum << endl;

	vector<vector<Point>> contours;
	MSER(mser_del, min_area, max_area, max_variation, min_diversity,
		max_evolution, area_threshold,
		min_margin, edge_blur_size) (img_edge, contours);

	for (int i = (int)contours.size() - 1; i >= 0; i--)
	{
		const vector<Point>& r = contours[i];
		RotatedRect box = fitEllipse(r);
		Rect box_rect = box.boundingRect();


		if (box_rect.x > 10)				// Empirical Adjustment on X-coord
			box_rect.x = box_rect.x - 8;

		if (box_rect.width > 20)			// Empirical Adjustment on width
			box_rect.width = box_rect.width + 15;

		if (box_rect.y > 10)				// Empirical Adjustment on Y-coord
			box_rect.y = box_rect.y - 5;

		if (box_rect.height > 20)			// Empirical Adjustment on height
			box_rect.height = box_rect.height + 5;


		// Edge case bounding of the words.

		if (box_rect.x < 0)
			box_rect.x = 0;
		else if (box_rect.x > img_inp.cols)
			box_rect.x = img_inp.cols;

		if (box_rect.y < 0)
			box_rect.y = 0;
		else if (box_rect.y > img_inp.rows)
			box_rect.y = img_inp.rows;

		if (box_rect.x + box_rect.width > img_inp.cols)
			box_rect.width = img_inp.cols - box_rect.x - 1;

		if (box_rect.y + box_rect.height > img_inp.rows)
			box_rect.height = img_inp.rows - box_rect.y - 1;

		// Pixel density check. Eliminates false samples.

		Mat pd_check = img_inp(box_rect);
		threshold(pd_check, pd_check, 100, 255, CV_THRESH_OTSU);
		float pd = 0;
		for (int i = 0; i < pd_check.rows; i++)
			for (int j = 0; j < pd_check.cols; j++)
				pd = pd + pd_check.at<uchar>(i, j);
		pd = 1 - (pd / (float)(256 * pd_check.rows*pd_check.cols));

		cout << "scissoring:aspectratio:task:" << tasknum << endl;

		// Scissored image aspect-ratio check. Elimiates false samples.
		float asp_rat = (box_rect.height / (float)box_rect.width);
		float box_area = (box_rect.height * box_rect.width);
		
		cout << asp_rat << endl;
		cout << box_area << endl;
		cout << pd << endl;


		if ((min_area < box_area) && (box_area < max_area) 
			&& (ASP_RAT_LOW < asp_rat) && (asp_rat < ASP_RAT_HIGH) && (pd > PIX_DEN_THRESH))
		{
			if ((0.85 < asp_rat) || (pd < 0.03))
			{
				Word tmp_word(img_inp(box_rect), Point(box_rect.x, box_rect.y), false); // Manual Re-check required.
				img_words.push_back(tmp_word);
			}
			else
			{
				Word tmp_word(img_inp(box_rect), Point(box_rect.x, box_rect.y), true); // Manual Re-check not required.
				img_words.push_back(tmp_word);
			}

		}
	}

	if (show_fig)
	{
		Mat tmp_img;
		img_inp.copyTo(tmp_img);
		for (int i = 0; i < img_words.size(); i++)
			rectangle(tmp_img, Rect(img_words.at(i).getOrigin(), img_words.at(i).getImage().size()), Scalar(0, 0, 0), 1);
		namedWindow("Scissored Image", WINDOW_NORMAL);
		imshow("Scissored Image", tmp_img);
		imwrite("ScissoredImage.jpg", tmp_img);
	}

	cout << "scissoring<<before return:task:" << tasknum << endl;

	return img_words;

}


vector<Word> word_crop::scissor_scale(Mat img_inp_rgb, int tasknum, bool show_fig = false, bool verbose = false)
{

	vector<Word> img_words;

	Mat img_inp;
	if (img_inp_rgb.channels() > 1)
		cvtColor(img_inp_rgb, img_inp, CV_BGR2GRAY);
	else
		img_inp_rgb.copyTo(img_inp);

	double scale_row = round(img_inp.rows / (double)rz_rows);
	double scale_col = round(img_inp.cols / (double)rz_cols);
	double scale_area = scale_row * scale_col;
	
	if (verbose)
	{
		cout << "Word Cropping...task:" <<tasknum<< endl;
		cout << "Row Scaling: " << scale_row << '\t'
			<< "Column Scaling: " << scale_col << '\t' << "Area Scaling: " << scale_area << endl;
	}

	Mat img_edge(Size(img_inp.size()), CV_8U);
	img_inp.copyTo(img_edge);
		
	//threshold(img_edge, img_edge, FILTER_THRESH, 255, THRESH_BINARY_INV);
	Canny(img_edge, img_edge, CANNY_LOW_THRESH, CANNY_HIGH_THRESH);
	Mat ele = getStructuringElement(MORPHO_ELEMENT, Size(MORPHELE_WIDTH_MULT * scale_col, MORPHELE_HEIGHT_MULT * scale_row), Point(-1, -1));
	dilate(img_edge, img_edge, ele, Point(-1, -1), DILATE_ITER);
	
	int r = GAUSS_WIN_ROW_multiplier * scale_row, c = GAUSS_WIN_COL_multiplier * scale_col;
	(r % 2 == 0) ? r -= 1 : NULL; (c % 2 == 0) ? c -= 1 : NULL;
	GaussianBlur(img_edge, img_edge, Size(c, r), GAUSS_VAR, GAUSS_VAR);

	if (show_fig)
	{
		namedWindow("Canny", WINDOW_NORMAL);
		imshow("Canny", img_edge);
		waitKey(0);
		destroyWindow("Canny");
		//imwrite("Dilated.jpg", img_edge);
		//cout << "Check Dilated.jpg" << endl;
	}


	vector<vector<Point>> contours;
	MSER(mser_del, min_area, max_area, max_variation, min_diversity,
		max_evolution, area_threshold,
		min_margin, edge_blur_size) (img_edge, contours);
	//MSER() (img_edge, contours);
	cout << "MSER Delta: " << mser_del << endl;
	
	for (int i = (int)contours.size() - 1; i >= 0; i--)
	{
		const vector<Point>& r = contours[i];
		RotatedRect box = fitEllipse(r);
		Rect box_rect = box.boundingRect();

		if (box_rect.x > 10)				// Empirical Adjustment on X-coord
			box_rect.x = box_rect.x - 8;

		if (box_rect.width > 20)			// Empirical Adjustment on width
			box_rect.width = box_rect.width + 15;

		if (box_rect.y > 10)				// Empirical Adjustment on Y-coord
			box_rect.y = box_rect.y - 5;

		if (box_rect.height > 20)			// Empirical Adjustment on height
			box_rect.height = box_rect.height + 5;


		// Edge case bounding of the words.

		if (box_rect.x < 0)
			box_rect.x = 0;
		else if (box_rect.x > img_inp.cols)
			box_rect.x = img_inp.cols;

		if (box_rect.y < 0)
			box_rect.y = 0;
		else if (box_rect.y > img_inp.rows)
			box_rect.y = img_inp.rows;

		if (box_rect.x + box_rect.width > img_inp.cols)
			box_rect.width = img_inp.cols - box_rect.x - 1;

		if (box_rect.y + box_rect.height > img_inp.rows)
			box_rect.height = img_inp.rows - box_rect.y - 1;

		if ((box_rect.width < 0) || (box_rect.height < 0))
			continue;

		// Pixel density check. Eliminates false samples.

		Mat pd_check = img_inp(box_rect);
		threshold(pd_check, pd_check, 0, 255, CV_THRESH_OTSU);
		float pd = 0;
		for (int i = 0; i < pd_check.rows; i++)
			for (int j = 0; j < pd_check.cols; j++)
				pd = pd + pd_check.at<uchar>(i, j);
		pd = 1 - (pd / (float)(256 * pd_check.rows*pd_check.cols));

		// Scissored image aspect-ratio check. Elimiates false samples.
		float asp_rat = (box_rect.height / (float)box_rect.width);
		float box_area = (box_rect.height * box_rect.width);

		//(min_area < box_area) && (box_area < max_area) &&
		if ((ASP_RAT_LOW < asp_rat) && (asp_rat < ASP_RAT_HIGH) && (pd > PIX_DEN_THRESH))
		{
			if ((0.85 < asp_rat) || (pd < 0.03))
			{
				Word tmp_word(img_inp(box_rect), Point(box_rect.x, box_rect.y), false); // Manual Re-check required.
				img_words.push_back(tmp_word);
				
				if (show_fig)
				{ 
					string wrd_url = ("wrd_"+ to_string(i) + ".jpg");
					cout << "writing wrd:" << wrd_url << "task:" << tasknum << endl;
					imwrite(wrd_url,tmp_word.getImage()); 
				}
			}
			else
			{
				Word tmp_word(img_inp(box_rect), Point(box_rect.x, box_rect.y), true); // Manual Re-check not required.
				img_words.push_back(tmp_word);

				if (show_fig)
				{
					string wrd_url = ("wrd_"+ to_string(i) + ".jpg");
					cout << "writing wrd:" << wrd_url <<"task:"<<tasknum <<endl;
					imwrite(wrd_url, tmp_word.getImage());

				}

			}
		}
	}

	if (show_fig)
	{
		Mat tmp_img;
		img_inp.copyTo(tmp_img);
		for (int i = 0; i < img_words.size(); i++)
			rectangle(tmp_img, Rect(img_words.at(i).getOrigin(), img_words.at(i).getImage().size()), Scalar(0, 0, 0), 3);
		
		destroyAllWindows();
		cout << "Check ScissoredImage.jpg" << endl;
		namedWindow("Scissored Image", WINDOW_NORMAL);
		imshow("Scissored Image", tmp_img);
		waitKey(0);
		//imwrite("ScissoredImage.jpg", tmp_img);
		
	}

	cout << "wordcropping:before return:task" << tasknum << endl;

	return img_words;

}


#endif