#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Point2f src[4];
Point2f dst[4];
int i = 0;
float w = 0, h = 0; // Width and height of image

float distance_new(Point2f a, Point2f b) {
	return sqrt( pow((b.x - a.x), 2) + pow((b.y - a.y), 2) );
}

void onMouse(int event, int x, int y, int flags, void* params) {

	// cout << "Hey\n";
	Mat *img = reinterpret_cast<Mat*>(params);
	switch (event) {

	case EVENT_LBUTTONDOWN: {

		if (i < 4)
		{
			int px_val = static_cast<int>(img->at<uchar>(Point(x, y)));
			cout << "Pixel Value : " << x << ", " << y << "\n";
			src[i] = Point2f(x, y);
			i++;
		}

	} break;

	}
}

int main() {

	// string path = "/home/prakank/opencv_test/learn_opencv/Resources/cards.jpg";
	string path = "cards.jpg";
	Mat img, imgWarp, matrix, img_temp;
	img = imread(path);
	img_temp = imread(path);
	if (img.empty()) {
		cout << "Image not found or unable to load";
		return -1;
	}
	cvtColor(img, img, COLOR_BGR2GRAY);
	cvtColor(img_temp, img_temp, COLOR_BGR2GRAY);
	imshow("Image", img);

	if (i < 4)setMouseCallback("Image", onMouse, reinterpret_cast<void*>(&img));
	int j = 0;
	while (i != 4) {
		if (j != i) {
			destroyWindow("Image");
			circle(img, src[j], 10, Scalar(0, 0, 255), FILLED);
			imshow("Image", img);
			j = i;
			if (i < 4)setMouseCallback("Image", onMouse, reinterpret_cast<void*>(&img));
		}
		waitKey(1);
	}

	if (i == 4) {
		w = distance_new(src[0], src[1]) + distance_new(src[2], src[3]);
		w /= 2;
		h = distance_new(src[0], src[2]) + distance_new(src[1], src[3]);
		h /= 2;
		w *= 1.2;
		h *= 1.5;
		dst[0] = {0.0f, 0.0f};
		dst[1] = {w, 0.0f};
		dst[2] = {0.0f, h};
		dst[3] = {w, h};
		matrix = getPerspectiveTransform(src, dst);
		warpPerspective(img_temp, imgWarp, matrix, Point(w, h));

		for (int i = 0; i < 4; i++)
		{
			circle(img, src[i], 10, Scalar(0, 0, 255), FILLED);
			// cout << src[i] << endl;
		}
		destroyWindow("Image");
		imshow("Points Selected", img);
		resize(imgWarp, imgWarp, Size(imgWarp.cols * 0.85, imgWarp.rows * 0.75));
		// resizeWindow("Final Image", img.cols, img.rows * 0.8);
		// namedWindow("Final Image", CV_WINDOW_AUTOSIZE);
		imshow("Final Image", imgWarp);
	}
	waitKey(0);
	return 0;
}
