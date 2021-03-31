#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include<iostream>
using namespace std;
using namespace cv;

Point2f src[4];
//array of Point2f variables corresponding to points selected in original image

Point2f dst[4];

//array of Point2f variables corresponding to points selected in transformed image

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


int main(int argc, char* argv[]){
    dst[0] = {472, 52};
    dst[1] = {950, 52};
    dst[2] = {472, 1030};
    dst[3] = {950, 1030};

    //string path = argv[1];
    

    VideoCapture cap("trafficvideo.mp4");
    Rect road(472,52, 478, 978);

    if(!cap.isOpened()){ cerr<<"error in opening video file\n"; return 1;}

    char c;

    Mat frame, frame_temp, frame_cropped, frame_warped, matrix, frame_prev, frame_next;
    
    cap>>frame_temp;
    if(frame_temp.empty()) {cap.release(); destroyAllWindows(); return 1;}
    
    cvtColor(frame_temp, frame_temp, COLOR_BGR2GRAY);
    imshow("frame_temp", frame_temp);
    
    if (i < 4) setMouseCallback("frame_temp", onMouse, reinterpret_cast<void*>(&frame_temp));
    int j = 0;
    
    //marking points as they are being selected
    while (i != 4) {
        if (j != i) {

            destroyWindow("frame_temp");
            circle(frame_temp, src[j],10, Scalar(0, 0, 255), FILLED);
            imshow("frame_temp", frame_temp);
            j = i;
            if (i < 4)setMouseCallback ("frame_temp",onMouse, reinterpret_cast<void*>(&frame_temp));
        }
        waitKey(1);
    }

    if (i == 4) {
        
        matrix = getPerspectiveTransform(src, dst);
        
        warpPerspective(frame_temp, frame_warped, matrix, frame_temp.size());                
        frame_cropped = frame_warped(road);
        // imshow("Points Selected", img);
        
        frame_prev = frame_cropped;
        destroyWindow("frame_temp");        
        imshow("Cropped Frame", frame_cropped);
        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}

    }
    
    int t = 1;
    vector<float> density;

    while(t++){  
        //cout<<"Hey\n";
        cap>>frame;
        
        if(t%5!=0){
            if(frame.empty()) break;
            continue;
        }
        if(t==50)destroyWindow("Cropped Frame");
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        
        if (frame.empty()) break;      
        warpPerspective(frame, frame_warped, matrix, frame.size());
        frame_cropped = frame_warped(road);      
        imshow("Cropped_frame", frame_cropped);    
        frame_next = frame_cropped.clone();
        
        // Here goes the Optical Flow part vvv

       
       
        Mat flow(frame_prev.size(), CV_32FC2);
        calcOpticalFlowFarneback(frame_prev, frame_next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        imshow("frame2", bgr);

        float threshold = 20;
        float count=0;
        Vec3b pixel_values;
        float value = 0;

        for (int i=0;i<bgr.rows;i++){
            for(int j=0;j<bgr.cols;j++){
                pixel_values = bgr.at<Vec3b>(i,j);
                value = (pixel_values[0] + pixel_values[1] + pixel_values[2])/3;
                if(value>=threshold)count++;
                
            }
        }
        float density_val =  float(count/float(bgr.rows*bgr.cols));
        density.push_back(density_val);

        // Optical Flow part ^^^
        c = (char)waitKey(25);      
        if(c == 27) break;
        frame_prev = frame_next.clone();

    }

    string filename = "Density_Values.csv";
    ofstream outputFile(filename);    
    outputFile << "Density Values\n";
    for (int i=0;i<density.size();i++){
        outputFile << density[i] <<"\n";
    }
    outputFile.close();

    cap.release();
    destroyAllWindows();

    return 0;
}
