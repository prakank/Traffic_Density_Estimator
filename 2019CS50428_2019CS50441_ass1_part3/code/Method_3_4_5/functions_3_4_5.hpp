#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<thread>
#include<chrono>
#include<boost/algorithm/string.hpp>
#include<boost/format.hpp> 
#include "dependencies_3_4_5.hpp"

using namespace std;
using namespace cv;
using namespace std::chrono;

int i = 0;
Point2f src[4];                //array of Point2f variables corresponding to points selected in original image
Point2f dst[4];                //array of Point2f variables corresponding to points selected in transformed image


bool is_number(const string& s)
{
    string::const_iterator it = s.begin();
    // if(*it=='-')it++;
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

struct Combined_long{
    int FrameNumber;
    int PartNumber;
    int Sum;
    int Area;
};

struct Combined_short{
    int FrameNumber;
    float Density;
};

bool Comparator_short(Combined_short a, Combined_short b){
    if(a.FrameNumber < b.FrameNumber) return true;
    return false;
}

bool Comparator_long(Combined_long a, Combined_long b){
    if(a.FrameNumber < b.FrameNumber) return true;
    if(a.FrameNumber > b.FrameNumber) return false;
    if(a.PartNumber < b.PartNumber) return true;
    return false;
}

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

