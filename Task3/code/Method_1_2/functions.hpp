#include<opencv2/opencv.hpp>
#include<cmath>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<map>
#include<chrono>
#include "dependencies_1_2.hpp"

using namespace std;
using namespace cv;
using namespace std::chrono;




/*DO NOT CHANGE THE FOLLOWING CODE*/


using namespace std;
using namespace cv;
using namespace std::chrono;

Point2f src[4];

float base_runtime;
vector<float> base_queue_density;
map<float, pair<float, float>> param_utility_map;

Point2f dst[4];

int i = 0;
int w = 0, h = 0;

float distance_new(Point2f a, Point2f b) {
	return sqrt( pow((b.x - a.x), 2) + pow((b.y - a.y), 2) );
}

void onMouse(int event, int x, int y, int flags, void* params) {

	Mat *frame_temp = reinterpret_cast<Mat*>(params);
	switch (event) {

    //stores information about points if left mouse button is clicked
	case EVENT_LBUTTONDOWN: {

		if (i < 4)
		{
			int px_val = static_cast<int>(frame_temp->at<uchar>(Point(x, y)));
			cout << "Pixel Value : " << x << ", " << y << "\n";
			src[i] = Point2f(x, y);
			i++;
		}

	} break;

	}
}