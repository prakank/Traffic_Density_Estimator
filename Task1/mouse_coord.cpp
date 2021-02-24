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

	string path = "/home/prakank/opencv_test/learn_opencv/Resources/empty.jpg";
	// string path = "empty.jpg";
	Mat img, imgWarp, matrix, img_temp, img_transformed, img_cropped;
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
			circle(img, src[j],10, Scalar(0, 0, 255), FILLED);
			imshow("Image", img);
			j = i;
			if (i < 4)setMouseCallback("Image",onMouse, reinterpret_cast<void*>(&img));
		}
		waitKey(1);
	}

	if (i == 4) {

		dst[0] = {472, 52};
		dst[1] = {950, 52};
		dst[2] = {472, 1030};
		dst[3] = {950, 1030};
		matrix = getPerspectiveTransform(src, dst);
		warpPerspective(img_temp, imgWarp, matrix, img_temp.size());

		Rect road(472,52, 478, 978);
		img_cropped = imgWarp(road);

		destroyWindow("Image");
		imshow("Projected Image", imgWarp);
		imshow("Cropped Image", img_cropped);
		imwrite("Projected Image.jpg", imgWarp);
		imwrite("Cropped Image.jpg", img_cropped);
	}

	waitKey(0);
	return 0;

}