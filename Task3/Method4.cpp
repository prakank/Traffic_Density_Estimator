#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<thread>
#include<chrono>
using namespace std;
using namespace cv;
using namespace std::chrono;


Mat ref_frame, ref_frame_cropped, ref_frame_warped, matrix;
vector<float> v1;
vector<float> v2;
int NumberOfThreads = 1;
int FramesToSkip = 5;
string Video = "";
int TotalFrames = 5736;
int Progress = 0;

struct Combined{
    int FrameNumber;
    float Density;
};

vector<Combined> v;

Point2f src[4]; //array of Point2f variables corresponding to points selected in original image
Point2f dst[4]; //array of Point2f variables corresponding to points selected in transformed image
Rect road(472,52, 478, 978);

int i = 0;
int w = 0, h = 0;
char Esc;
bool Escaped = false;

bool is_number(const string& s)
{
    string::const_iterator it = s.begin();
    // if(*it=='-')it++;
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
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

void BackgroundSubtraction(int ThreadNumber){

    Mat frame, frame_warped, frame_cropped, diff_frame, diff_frame_thresh;
    VideoCapture cap(Video);
    int t = 1;
    while(t++){
        cap >> frame;
        if(t%FramesToSkip!=0){
            if(frame.empty()) break;
            continue;
        }
        if (frame.empty()) break;

        int quotient = t/FramesToSkip;
        if(quotient%NumberOfThreads != (ThreadNumber-1))continue;
                
        float sum = 0;

        cvtColor(frame, frame, COLOR_BGR2GRAY);
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

        Combined cd;
        cd.FrameNumber = t;
        cd.Density = sum/(frame_cropped.size().height * frame_cropped.size().width);
        
        v.push_back(cd);
        
        Esc = (char)waitKey(25);
        if(Esc == 27){cap.release(); destroyAllWindows(); return;}
        
        Progress = max(Progress, t);
        cout << "Progress: " << (int)Progress*100/TotalFrames << "%    " << '\r' << flush;
    }
}

void Output(){
    string filename = "Density_Values_M4_" + to_string(NumberOfThreads) + ".csv";
    ofstream outputFile(filename);
    
    outputFile << "FrameNumber, Density\n";
    for(int i = 0; i< v.size(); i++){
        outputFile << i << ", " << v[i].FrameNumber << ", " << v[i].Density << "\n";
    }    
    outputFile.close();
}

int main(int argc, char* argv[]){
    
    if(argc != 3){
        cout << "INVALID INPUT FORMAT\nCORRECT FORMAT: ./M3 <VideoPath> <NumberOfThreads>\n";
        return -1;
    }

    auto start = high_resolution_clock::now();
    Video = argv[1];
    // Video = "trafficvideo.mp4";

    char c;
    Mat frame, frame_temp, frame_cropped, frame_warped;
        
    /*code for generating cropped frame and Homography_matrix*/
    dst[0] = {472, 52};
    dst[1] = {950, 52};
    dst[2] = {472, 1030};
    dst[3] = {950, 1030};    
    
    VideoCapture cap(Video);
    
    if(!cap.isOpened()) {
        cout<<"Error in opening video file\n";
        return -1;
    }
        
    if(!is_number(argv[2])){
        cout << "INVALID INPUT FORMAT\nNumber of Threads should be a POSITIVE INTEGER\n";
        return -1;
    }
    NumberOfThreads = stoi(argv[2]);

    ref_frame = imread("Empty_frame.jpg");
    cvtColor(ref_frame, ref_frame, COLOR_BGR2GRAY);
    
    cap>>frame_temp;
    if(frame_temp.empty()) {cap.release(); destroyAllWindows(); return 1;}
    
    cvtColor(frame_temp, frame_temp, COLOR_BGR2GRAY);
    imshow("frame_temp", frame_temp);
    
    if (i < 4) setMouseCallback("frame_temp", onMouse, reinterpret_cast<void*>(&frame_temp));
    int j = 0;
    
    //Marking points as they are being selected
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
        destroyWindow("frame_temp");
        
        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}

    }
    /*end of code block*/
    
    cout << "Processing method 4 of 4 ...\n";
    vector <thread> threads;
    
    for(int p=0; p < NumberOfThreads; p++){
        threads.push_back(thread(BackgroundSubtraction, p+1));
    }

    for(int i=0;i<NumberOfThreads; i++)threads[i].join();
    cout << "Completed 100%\n";
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken: "<< duration.count() << " seconds" << endl;
    cout << "Number of threads: " << NumberOfThreads << endl;
    
    Output(); // Write output in a csv file
    

}