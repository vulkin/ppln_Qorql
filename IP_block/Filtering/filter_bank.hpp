#ifndef FILTERS_FILTERCLASS
#define FILTERS_FILTERCLASS

// Filter Bank: This class consists filtering methods.
// Created by: Gautam Krishna & Brijesh Dhave.
// Version: 0.3
// Qorql

#include "../qorql_IP.hpp"

using namespace std;
using namespace cv;



class filter_class
{
private:
	//public:

	static Mat filter_clahe(Mat inp_img, int clip_limit = 4)
	{
		// Static method implementing Contrast Limited Adaptive Histogram Equalization (CLAHE).
		// This is a modified version of Adaptive Histogram Equalization (AHE).

		// Input: Image mat object, clip limit value (clip_limit).
		// Output: Image mat object (out_img)
		// Defaults: None

		Mat out_img;
		Ptr<cv::CLAHE> clahe_filt = createCLAHE(clip_limit);
		clahe_filt->apply(inp_img, out_img);
		return out_img;
	}

	static Mat filter_MorphClose(Mat inp_img, int cols = rz_cols * 0.0125, int rows = rz_rows * 0.00390625, int iter = 1)
	{
		// Static method implementing morphological closing to the input image.
		// Closing helps in removing the letters in an image and leaving only background. When this
		// is subtracted from the original image, the resulting image is enhanced image.

		// Input: Image mat object (inp_img), number of operation iterations (iter).
		// Output: Image mat object (out_img).
		// Defaults: Scaling factors for image resolution and morphological element size (0.0125 and 0.00390625).

		// Empirical values for the morphological element are chosen, according to the image resolution.
		Mat out_img;
		Mat ele = getStructuringElement(MORPH_ELLIPSE, Size(cols, rows));
		morphologyEx(inp_img, out_img, MORPH_CLOSE, ele, Point(-1, -1), iter);
		return out_img;
	}


	static Mat filter_MorphOpen(Mat inp_img, float cols = rz_cols * 0.0125, float rows = rz_rows * 0.00390625, int iter = 1)
	{
		// Static method implementing morphological opening to the input image.
		// Closing helps in removing the letters in an image and leaving only background. When this
		// is subtracted from the original image, the resulting image is enhanced image.

		// Input: Image mat object (inp_img), number of operation iterations (iter).
		// Output: Image mat object (out_img).
		// Defaults: Scaling factors for image resolution and morphological element size (0.0125 and 0.00390625).

		// Empirical values for the morphological element are chosen, according to the image resolution.
		Mat out_img;
		(cols < 1) ? cols = 1 : NULL;
		(rows < 1) ? rows = 1 : NULL;
		Mat ele = getStructuringElement(MORPH_ELLIPSE, Size((int)cols, (int)rows));
		morphologyEx(inp_img, out_img, MORPH_OPEN, ele, Point(-1, -1), iter);
		return out_img;
	}

	static Mat filter_sharp(Mat inp_img, int blur_level = 2, int kernel = 5, float mult_inp = 1.0, float mult_blur = -0.3)
	{
		// Static method to sharpen the image using unsharp masking. This method uses Gaussian blur to
		// blur the image and subtracts it from the original.

		// Input: Image mat object (inp_img), gaussian blur variance (blur_level), gaussian kernel size (kernel),
		// input image weight (mult_inp) and blurred image weight (mult_blur)
		// Output: Sharpened image (out_img).
		// Defaults: Gaussian variance = 2, Gaussian Kernel Size = 5, Input image weight = 1.0, Blurred Image weight = -0.3
		Mat blur_img, out_img;

		GaussianBlur(inp_img, blur_img, Size(kernel, kernel), blur_level, blur_level);
		addWeighted(inp_img, mult_inp, blur_img, mult_blur, 0, out_img);
		return out_img;
	}

	static Mat filter_RemWhiteBackgrnd(Mat inp_img, int morph_cols = 0, int morph_rows = 0, int iter = 1, int filter_type = FILTER_BCKGND_UNITY)
	{
		// Static method to remove noisy WHITE background in an image. This method use morphological opening and subtracts the
		// image from the original. This results in removing the background.
		// This is iteratively done, resulting in a better overall image.

		// Input: Image mat object (inp_img), morphological element size (morph_rows, morph_cols), number of iterations (iter),
		// and the type of amplification required after the background subtraction.
		// Output: Image with enhanced background.
		// Defaults: 

		if (iter < 1)
			iter = 1;
		if (morph_cols == 0)
			morph_cols = (inp_img.cols * 10) / 720;
		if (morph_rows == 0)
			morph_rows = (inp_img.rows * 10) / 1280;

		Mat neg_im = 255 - inp_img;

		for (int i = 0; i < iter; i++)
		{
			Mat bkgnd = filter_MorphOpen(neg_im, morph_cols, morph_rows);

			if (filter_type == FILTER_BCKGND_CONST)
				neg_im = iter*(neg_im - bkgnd);
			else if (filter_type == FILTER_BCKGND_LINEAR)
				neg_im = i*(neg_im - bkgnd);
			else if (filter_type == FILTER_BCKGND_UNITY)
				neg_im = neg_im - bkgnd;
		}

		int sz;
		(inp_img.rows > inp_img.cols) ? sz = inp_img.cols / 100 : sz = inp_img.rows / 100;
		(sz % 2 == 0) ? sz = sz - 1 : 0;
		//medianBlur(neg_im, neg_im, sz);
		threshold(neg_im, neg_im, 50, 255, THRESH_TOZERO);

		return (255 - neg_im);
	}


	static void filter_imwrite(Mat inp_mat, string file_name)
	{
		// Static method to write an image to a 'png' file.

		// Input: Image mat object (inp_img), file name (without extension) of the image to be saved.
		// Output: None.
		// Defaults: None.

		file_name.append(".png");
		imwrite(file_name, inp_mat);
	}

	static Mat filter_BnC(Mat inp_mat, float alpha = 1, float beta = 0)
	{
		Mat new_image = Mat::zeros(inp_mat.size(), inp_mat.type());

		(0 > alpha) ? alpha = 0 : 0;
		(alpha > 3.0) ? alpha = 3.0 : 0;
		(0 > beta) ? beta = 0 : 0;
		(beta > 100) ? beta = 100 : 0;

		for (int y = 0; y < inp_mat.rows; y++)
		{
			for (int x = 0; x < inp_mat.cols; x++)
			{
				if (inp_mat.channels() > 1)
					for (int c = 0; c < 3; c++)
						new_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(inp_mat.at<Vec3b>(y, x)[c]) + beta);
				else
					new_image.at<uchar>(y, x) = saturate_cast<uchar>(alpha*(inp_mat.at<uchar>(y, x)) + beta);
			}
		}

		return new_image;
	}


public:

	// Public methods to use the above modules and make a filter.
	filter_class(){}
	static Mat filter_brijesh(Mat, bool);
	static Mat filter_gautam(Mat,int, bool, bool);
	static Mat filter_gautam2(Mat, bool);
	static Mat filter_enhanced(Mat, bool);
	static Mat filter_brijesh2(Mat);
	static Mat filter_gautam_track(Mat, bool);
	static Mat filter_gautam_ver1(Mat, bool);

};



//  "filter_class" definitions.



Mat filter_class::filter_gautam_ver1(Mat inp_img, bool disp_img = false)
{

	// Mat Check
	if (!inp_img.data)
	{
		cerr << "No Image date found. Terminating execution" << endl;
		waitKey(1000);
		exit(0);
	}


	// Channel check and resize.
	if (inp_img.channels()>1)
		cvtColor(inp_img, inp_img, CV_BGR2GRAY);
	resize(inp_img, inp_img, Size(), 2, 2, INTER_LANCZOS4);


	Mat bkgnd, neg_img, tmp_img;

	// Background Removal.
	neg_img = 255 - inp_img;
	bkgnd = filter_MorphOpen(neg_img, inp_img.cols * 0.0125, inp_img.rows * 0.00390625);// , 10, 10);
	neg_img = neg_img - bkgnd;

	if (disp_img)
	{
		imshow("Backgorund Elimination", neg_img);
		//waitKey(0);
	}

	Mat neg_brnch;
	neg_img.copyTo(neg_brnch);
	bkgnd = filter_MorphOpen(neg_brnch, inp_img.cols * 0.0125, inp_img.rows * 0.00390625);
	neg_brnch = 5 * (neg_brnch - bkgnd);

	threshold(neg_brnch, neg_brnch, 100, 255, THRESH_TOZERO);
	neg_brnch = 255 - neg_brnch;

	char s = 'n';
	cout << "Save Image (Y/N): ";
	//cin >> s;
	if ((s == 'y') || (s == 'Y'))
		filter_imwrite(neg_brnch, "SavedImage");
	cout << endl;

	return neg_brnch;

}


//  "filter_class" definitions.
Mat filter_class::filter_gautam(Mat inp_img,int tasknum=666, bool disp_img = false, bool verbose = false)
{

	// Mat Check
	if (!inp_img.data)
	{
		cerr << "No Image date found. Terminating execution" << endl;
		waitKey(1000);
		exit(0);
	}


	// Channel check and resize.
	if (inp_img.channels()>1)
		cvtColor(inp_img, inp_img, CV_BGR2GRAY);
	//resize(inp_img, inp_img, Size(rz_cols, rz_rows));
	//resize(inp_img, inp_img, Size(), 2, 2, INTER_LANCZOS4);

	float scale_row = inp_img.rows / (float)rz_rows;
	float scale_col = inp_img.cols / (float)rz_cols;
	float scale_area = scale_row * scale_col;

	if (verbose)
	{
		cout << "Filtering the image..." << endl;
		cout << "Row Scaling: " << scale_row << '\t'
			<< "Column Scaling: " << scale_col << '\t' << "Area Scaling: " << scale_area << endl;
	}

	Mat bkgnd, neg_img, tmp_img;

	// Background Removal.
	cout << "filt:bgremove start:task:" << tasknum << endl;
	neg_img = 255 - inp_img;
	bkgnd = filter_MorphOpen(neg_img, scale_col * MorphOpen_Col_multiplier, scale_row * MorphOpen_Row_multiplier);
	neg_img = neg_img - bkgnd;

	if (disp_img)
	{
		namedWindow("Background Elimination", WINDOW_NORMAL);
		imshow("Background Elimination", neg_img);
		waitKey(10);
		destroyWindow("Background Elimination");
		imwrite("BG_Eliminate_P1.jpg", neg_img);
	}

	Mat neg_brnch;
	neg_img.copyTo(neg_brnch);
	bkgnd = filter_MorphOpen(neg_brnch, scale_col * MorphOpen_Col_multiplier, scale_row * MorphOpen_Row_multiplier);
	neg_brnch = difference_multiplier * (neg_brnch - bkgnd);

	cout << "filt:bgremove end:task:" << tasknum << endl;

	if (disp_img)
	{
		namedWindow("Background Elimination Phase 2", WINDOW_NORMAL);
		imshow("Background Elimination Phase 2", neg_brnch);
		waitKey(10);
		destroyWindow("Background Elimination Phase 2");
		imwrite("BG_Eliminate_P2.jpg", neg_brnch);
	}

	cout << "filt:threshold:task:" << tasknum << endl;
	threshold(neg_brnch, neg_brnch, Filter_threshold, 255, THRESH_TOZERO);
	//threshold(neg_brnch, neg_brnch, 0, 255, THRESH_OTSU);
	neg_brnch = 255 - neg_brnch;

	int tmp = 3 * scale_area;
	(tmp % 2 == 0) ? tmp -= 1 : 0;
	//medianBlur(neg_brnch, neg_brnch, tmp);
	//resize(inp_img, inp_img, Size(), 0.5, 0.5, INTER_LANCZOS4);

	if (disp_img)
	{
		imwrite("filt_final_P3.jpg", neg_brnch);
	}

	cout << "before filt returns:task:" <<tasknum<< endl;
	return neg_brnch;

}



// Advantages: Filters hadnwritten images to appreciable level.
// Dis-Advans: Distorts image to a lot extent too. Too delicate to word level distortions. Over-filters the image
//             resulting in broken text.
Mat filter_class::filter_gautam2(Mat inp_img, bool disp_img = false)
{

	// Mat Check
	if (!inp_img.data)
	{
		cerr << "No Image date found. Terminating execution" << endl;
		waitKey(1000);
		exit(0);
	}


	// Channel check and resize.
	if (inp_img.channels()>1)
		cvtColor(inp_img, inp_img, CV_BGR2GRAY);
	resize(inp_img, inp_img, Size(rz_cols, rz_rows));


	Mat bkgnd, neg_img, tmp_img;

	// Background Removal.
	neg_img = 255 - inp_img;
	bkgnd = filter_MorphOpen(neg_img, 10, 10);
	neg_img = neg_img - bkgnd;

	if (disp_img)
	{
		imshow("Backgorund Elimination", neg_img);
		waitKey(0);
	}


	// Further Denoising.
	bilateralFilter(neg_img, tmp_img, 0, 3, 15);
	adaptiveThreshold(tmp_img, tmp_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 3, 0);
	medianBlur(tmp_img, tmp_img, 3);
	medianBlur(neg_img, neg_img, 3);

	Mat cool_toad = 0.2*tmp_img + neg_img;
	threshold(cool_toad, cool_toad, 10, 255, THRESH_BINARY);

	cool_toad = 255 - cool_toad;
	Mat ele = getStructuringElement(MORPH_RECT, Size(6, 3));
	dilate(cool_toad, cool_toad, ele);

	return cool_toad;

}


// Obsolete Filters
Mat filter_class::filter_brijesh(Mat inp_img, bool disp_img = false)
{

	// Mat Check
	if (!inp_img.data)
	{
		cerr << "No Image date found. Terminating execution" << endl;
		waitKey(1000);
		exit(0);
	}


	// Channel check and resize.
	if (inp_img.channels() > 1)
		cvtColor(inp_img, inp_img, CV_RGB2GRAY);
	resize(inp_img, inp_img, Size(rz_cols, rz_rows));

	// Background removal.
	Mat bkgnd, ele, neg_img;
	neg_img = 255 - inp_img; // Image Negative
	bkgnd = filter_MorphOpen(neg_img, 30, 30);
	neg_img = neg_img - bkgnd;


	// Post Proccessing: De-Noising.
	Mat avg_img, tmp_img;
	blur(neg_img, avg_img, Size(3, 3));
	bilateralFilter(avg_img, tmp_img, 5, 10, 5);
	medianBlur(tmp_img, avg_img, 3);

	Mat pos_img = 255 - (2 * neg_img + avg_img);
	pos_img = filter_sharp(pos_img, 1.5, 5, 1, -0.25);

	GaussianBlur(pos_img, tmp_img, Size(5, 5), 0);
	medianBlur(tmp_img, tmp_img, 5);
	addWeighted(pos_img, 1.5, tmp_img, -0.25, 0, pos_img);

	pos_img = filter_clahe(pos_img, 8);
	pos_img = filter_sharp(pos_img, 1.5, 5, 1, -0.25);

	if (disp_img)
	{
		imshow("Brijesh Filtering", pos_img);
		waitKey(0);
	}
	return pos_img;
}

Mat filter_class::filter_brijesh2(Mat img_rgb)
{
	Mat img_gray;

	// Converting to Grayscale
	if (img_rgb.channels() > 1)
		cvtColor(img_rgb, img_gray, CV_RGB2GRAY);

	// Enhancing the contrast in the image
	Mat img_clahe;
	img_clahe = filter_clahe(img_gray);

	Mat img_neg, img_back;
	img_gray = img_clahe;

	// Converting the image to its negative
	// Doing the Opening operation so that all small part images(i.e. words) are removed and we just get the background
	img_neg = 255 - img_gray;
	Mat background;
	background = filter_MorphOpen(img_neg, 30, 30);
	img_neg = img_neg - background;


	// further removal of noise by averaging and median filtering
	Mat img_avg, img_x;
	blur(img_neg, img_avg, Size(3, 3));
	bilateralFilter(img_avg, img_x, 5, 10, 5);
	medianBlur(img_x, img_avg, 3);
	img_neg = 2 * img_neg + img_avg;

	Mat img_new, img_blur;
	img_new = 255 - img_neg;
	img_new = filter_sharp(img_new, 0, 5, 1.5, -0.25);

	GaussianBlur(img_new, img_blur, Size(5, 5), 0);
	medianBlur(img_blur, img_blur, 5);
	addWeighted(img_new, 1.5, img_blur, -0.25, 0.0, img_new);

	img_new = filter_clahe(img_new, 8);
	img_new = filter_sharp(img_new, 0, 5, 1.5, -0.25);


	return img_new;

	//imwrite(format("/Users/brijesh/desktop/Data_base/pp%d.jpg",i), img_new);
}


#endif