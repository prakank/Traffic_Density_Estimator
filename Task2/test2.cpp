#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
using namespace std;
using namespace cv;

Point2f src[4];
//array of Point2f variables corresponding to points selected in original image

Point2f dst[4];

//array of Point2f variables corresponding to points selected in transformed image

int i = 0;
int w = 0, h = 0;

const char* params
    = "{ help h         |           | Print usage }"
      "{ input          | vtest.avi | Path to a video or a sequence of image }"
      "{ algo           | MOG2      | Background subtraction method (KNN, MOG2) }";

float distance_new(Point2f a, Point2f b) {
	return sqrt( pow((b.x - a.x), 2) + pow((b.y - a.y), 2) );
}

void onMouse(int event, int x, int y, int flags, void* params) {

	// cout << "Hey\n";
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
    
    CommandLineParser parser(argc, argv, params);
    parser.about("Program for generating queue density and motion density plots");
     parser.printMessage();
    
    
    
    char c;

    Mat frame, frame_temp, frame_cropped, frame_warped, matrix, ref_frame, ref_frame_cropped, ref_frame_warped, diff_frame, diff_frame_thresh, blur_dst;
    
    
    
    /*code for generating cropped frame and Homography_matrix*/
    dst[0] = {472, 52};
    dst[1] = {950, 52};
    dst[2] = {472, 1030};
    dst[3] = {950, 1030};

    //string path = argv[1];
    

    VideoCapture cap("trafficvideo.mp4");
    Rect road(472,52, 478, 978);

    if(!cap.isOpened()) cerr<<"error in opening video file\n";

    
    
    ref_frame = imread("Empty_frame.jpg");
    cvtColor(ref_frame, ref_frame, COLOR_BGR2GRAY);
    
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
        
        warpPerspective(ref_frame, ref_frame_warped, matrix, ref_frame.size());
        
        ref_frame_cropped = ref_frame_warped(road);
        
        frame_cropped = frame_warped(road);
        
        // imshow("Points Selected", img);
        
        destroyWindow("frame_temp");
        
        imshow("Cropped Frame", frame_cropped);
        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}

    }
    /*end of code block*/
    
    
    /*code for processing video frames*/
    
    //Ptr<BackgroundSubtractor> pBackSub;
    //pBackSub = createBackgroundSubtractorMOG2();
    //pBackSub = createBackgroundSubtractorKNN();
    Mat fgMask;
    int t = 1;
    string filename = "Density_values2.csv";
    ofstream outputFile(filename);
    outputFile<< "Density Values2\n";
    while(t++){
        
        cap>>frame;
        //pBackSub->apply(frame, fgMask);
        
        if(t%5!=0){
        if(frame.empty()) break;
        continue;
        }
    
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        if (frame.empty()) break;
      
        float sum = 0;
        warpPerspective(frame, frame_warped, matrix, frame.size());
        frame_cropped = frame_warped(road);
        
        absdiff(frame_cropped, ref_frame_cropped, diff_frame);
        threshold(diff_frame, diff_frame_thresh, 40, 127, 0);
      
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        
        findContours(diff_frame_thresh, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Rect> boundRect(contours.size());
        
        for(int k = 0;k<contours.size();k++){
            int area = contourArea(contours[k]);
            sum = sum+area;
        }
        
        outputFile<<sum/(frame_cropped.size().height*frame_cropped.size().width)<<"\n";
        
        cout<<sum/(frame_cropped.size().height*frame_cropped.size().width)<<"\n";
                                     
        imshow("Cropped_frame", diff_frame_thresh);
    
        //if(t==4855) imwrite("Empty_frame.jpg", frame);
      
        cout<<t<<"\n";
      
    
    
        c = (char)waitKey(25);
        if(c == 27) break;
    }

    outputFile.close();
    
    cap.release();

    destroyAllWindows();

  
}
