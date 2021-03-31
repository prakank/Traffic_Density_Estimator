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
int NumberOfThreads = 1;
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
vector<float> fin;
Point2f src[4]; //array of Point2f variables corresponding to points selected in original image
Point2f dst[4]; //array of Point2f variables corresponding to points selected in transformed image
Rect road(472,52, 478, 978);
int TotalTimeTaken1;
int TotalTimeTaken2;
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

bool Comparator_long(Combined a, Combined b){
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

void BackgroundSubtraction(Rect part, int PartNumber){

    Mat frame, frame_warped, frame_cropped, frame_cropped_subpart, ref_frame_cropped_subpart, diff_frame, diff_frame_thresh;
    VideoCapture cap(Video);
    int t = 1;
    while(t++){
        cap >> frame;
        if(t%FramesToSkip!=0){
            if(frame.empty()) break;
            continue;
        }
        if (frame.empty()) break;
        
        float sum = 0;
        
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        warpPerspective(frame, frame_warped, matrix, frame.size());
        frame_cropped = frame_warped(road);
        frame_cropped_subpart = frame_cropped(part);
        ref_frame_cropped_subpart = ref_frame_cropped(part);
        absdiff(frame_cropped_subpart, ref_frame_cropped_subpart, diff_frame);
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
        cd.PartNumber = PartNumber;
        cd.FrameNumber = t;
        cd.Sum = sum;
        cd.Area = frame_cropped_subpart.size().height * frame_cropped_subpart.size().width;
        v.push_back(cd);
        Esc = (char)waitKey(25);
        if(Esc == 27){cap.release(); destroyAllWindows(); return;}
        
        Progress = max(Progress, t);
        cout << "Progress: " << (int)Progress*100/TotalFrames << "%    " << '\r' << flush;
    }
}

void Output(){

    sort(v.begin(), v.end(),Comparator_long);

    string filename = "Density_Values_M3.csv";
    ofstream outputFile(filename);    
    outputFile << "FrameNumber,Density Values\n";
    outputFile << NumberOfThreads << ", " << to_string(TotalTimeTaken1) << "_" << to_string(TotalTimeTaken2) << "\n";
    
    for(int i=0;i<v.size()/NumberOfThreads;i++){
        int sum=0;
        int area=0;
        for(int j=0;j<NumberOfThreads;j++){
            int in = i*NumberOfThreads + j;
            sum+=v[in].Sum;
            area+=v[in].Area;
        }
        float dens = float((float)sum/(float)area);
        fin.push_back(dens);
        outputFile << i+1 << "," << dens << "\n";
    }

    outputFile.close();
}

int main(int argc, char* argv[]){
    
    if(argc != 3){
        cout << "INVALID INPUT FORMAT\nCORRECT FORMAT: ./method3 <VideoPath> <NumberOfThreads>\n";
        return -1;
    }

    
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
    
    cout << "\nProcessing method 3 of 5 ...\n";
    cout << "Generating Queue Density with Number of Threads = " << NumberOfThreads <<" ...\n";
    auto start = high_resolution_clock::now();
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
    cout << "Completed 100%\n";    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    TotalTimeTaken1 = duration.count();
    
    cout << "Time taken: "<< duration.count() << " seconds" << endl;


    // Below Method is for generating Baseline Queue Values


    start = high_resolution_clock::now();
    VideoCapture cap2(Video);
    cout<<"\nGenerating Baseline Queue Density ...\n";        
    bool escaped = false;
    int t = 1;
    while(t++){

        // if(t%57 == 0) progress = t*100 /TotalFrames;
        // if(progress == 0) progress = 1;
        cout<<"Progress: "<< (int)t*100 /TotalFrames <<"%    "<<'\r'<<flush;
        
        cap2 >> frame;
        
        if(t%FramesToSkip!=0){
            if(frame.empty()) break;
            continue;
        }
    
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        if (frame.empty()) break;
      
        float sum = 0;
        warpPerspective(frame, frame_warped, matrix, frame.size());
        frame_cropped = frame_warped(road);
        
        Mat diff_frame, diff_frame_thresh;
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
        
        v1.push_back(sum/(frame_cropped.size().height*frame_cropped.size().width));
            
        c = (char)waitKey(25);
        if(c == 27){escaped = true; break;}
    }
    
    if(!escaped) cout<<"Completed 100%\n";

    cap.release();

    destroyAllWindows();

    stop = high_resolution_clock::now();
    duration = duration_cast<seconds>(stop - start);
    TotalTimeTaken2 = duration.count();
    cout << "Time taken: "<< TotalTimeTaken2 << " seconds" << endl;

    Output(); // Write output  of Method 3 in a csv file
    // Called this function later so as to obtain the value of TotalTimeTaken2

    // Write output  of Baseline Queue in a csv file
    ofstream outputFile;
    outputFile.open("Density_Values_Baseline_Queue.csv");    
    outputFile << "Density Values\n";
    for(int i = 0; i< v1.size(); i++){
        outputFile << i << ", " << v1[i] << "\n";            
    }    
    outputFile.close();

    cout << "\nError Computation ...\n";

    float err=0;
    float utility=0;
    for(int i=0;i<min(fin.size(), v1.size());i++){
        float val = (fin[i] - v1[i])*(fin[i] - v1[i]);
        err+=val;
    }
    err/=min(fin.size(), v1.size());
    err = sqrt(err);
    utility = log2(1/err);
    
    cout << "RMS Error:" << err << "\n";
    cout << "Utility (log[1/error]):" << utility << "\n";

}