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
int NumberOfThreads = 3;
int FramesToSkip = 5;
string Video = "";
int TotalFrames = 5736;
int Progress = 0;

struct Combined{
    int FrameNumber;
    int PartNumber;
    int Sum;
    int Area;
};

vector<Combined> v;

Point2f src[4]; //array of Point2f variables corresponding to points selected in original image
Point2f dst[4]; //array of Point2f variables corresponding to points selected in transformed image
Rect road(472,52, 478, 978);

int i = 0;
int w = 0, h = 0;
char Esc;
bool Escaped = false;

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

void BackgroundSubtraction(Rect part, int PartNumber){

    Mat frame, frame_warped, frame_cropped, frame_cropped_subpart, ref_frame_cropped_subpart, diff_frame, diff_frame_thresh;
    VideoCapture cap(Video);
    int t = 1;
    while(t++){
        cap >> frame;
        // imshow("Or",frame);
        // waitKey(100);
        // cout << frame.size();
        if(t%FramesToSkip!=0){
            if(frame.empty()) break;
            continue;
        }
        if (frame.empty()) break;
        
        

        float sum = 0;
        try{
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, frame_warped, matrix, frame.size());
            frame_cropped = frame_warped(road);
            frame_cropped_subpart = frame_cropped(part);
            ref_frame_cropped_subpart = ref_frame_cropped(part);
            absdiff(frame_cropped_subpart, ref_frame_cropped_subpart, diff_frame);
            threshold(diff_frame, diff_frame_thresh, 40, 127, 0);
        }
        catch(int x){
            cout << "Error\n";
        }
        
        
        // imshow("frame_cropped_subpart" + to_string(PartNumber), frame_cropped_subpart);
        // imshow("ref_frame_cropped_subpart" + to_string(PartNumber), ref_frame_cropped_subpart);
        // imshow("frame_cropped" + to_string(PartNumber), frame_cropped);
        // imshow("ref_frame_cropped" + to_string(PartNumber), ref_frame_cropped);
        // waitKey(1000);


        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        
        findContours(diff_frame_thresh, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Rect> boundRect(contours.size());
        
        for(int k = 0;k<contours.size();k++){
            int area = contourArea(contours[k]);
            sum = sum+area;
        }
        Combined cd;
        cd.PartNumber = PartNumber;
        cd.FrameNumber = t;
        cd.Sum = sum;
        cd.Area = frame_cropped_subpart.size().height * frame_cropped_subpart.size().width;
        v.push_back(cd);
        Esc = (char)waitKey(25);
        if(Esc == 27){Escaped = true; break;}
        // cout << t <<"\n";
        Progress = max(Progress, t);
        cout << "Progress: " << (int)Progress*100/TotalFrames << "%    " << '\r' << flush;
    }
}

int main(int argc, char* argv[]){
    
    auto start = high_resolution_clock::now();
    // string s = argv[1];
    Video = "trafficvideo.mp4";
    char c;
    Mat frame, frame_temp, frame_cropped, frame_warped;
        
    /*code for generating cropped frame and Homography_matrix*/
    dst[0] = {472, 52};
    dst[1] = {950, 52};
    dst[2] = {472, 1030};
    dst[3] = {950, 1030};    
    
    VideoCapture cap(Video);
    

    if(!cap.isOpened()) cerr<<"Error in opening video file\n";
        
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
        // frame_prev = frame_cropped;
                
        destroyWindow("frame_temp");
        
        /*imshow("Cropped Frame", frame_cropped);*/
        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}

    }
    /*end of code block*/
    
    
    vector <thread> threads;
    
    for(int p=0; p < NumberOfThreads; p++){
        int width = ref_frame_cropped.size().width;
        int height;

        if(p!=NumberOfThreads-1)height = ref_frame_cropped.size().height/NumberOfThreads;
        else height = ref_frame_cropped.size().height - p*ref_frame_cropped.size().height/NumberOfThreads;

        Rect cropped_part_split(0, p*ref_frame_cropped.size().height/NumberOfThreads, 478, height);

        // cout << i*ref_frame_cropped.size().height/NumberOfThreads << "    " << height << endl;
        threads.push_back(thread(BackgroundSubtraction, cropped_part_split, p));
    }

    for(int i=0;i<NumberOfThreads; i++)threads[i].join();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken by function: "<< duration.count() << " seconds" << endl;
    cout << "Number of threads:" << NumberOfThreads << endl;

    string filename = "Density_Values_" + to_string(NumberOfThreads) + ".csv";
    ofstream outputFile(filename);
    
    outputFile << "Frame, Part, Sum, Area\n";
    // for(int i = 1; i*14.45/5 < min(v1.size(), v2.size()); i++){
    //     outputFile << i << ", "<<v1[i*14.45/5] <<", "<<v2[i*14.45/5]<<"\n";
    // }
    for(int i = 0; i< v.size(); i++){
        outputFile << i << ", " << v[i].FrameNumber << ", " << v[i].PartNumber << ", " << v[i].Sum << ", " << v[i].Area << "\n";
    }
    
    outputFile.close();

}
