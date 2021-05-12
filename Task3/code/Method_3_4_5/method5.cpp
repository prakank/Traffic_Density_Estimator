#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include<iostream>
#include<chrono>
using namespace std;
using namespace cv;
using namespace std::chrono;

#define pb push_back

Point2f src[4];
//array of Point2f variables corresponding to points selected in original image

Point2f dst[4];

//array of Point2f variables corresponding to points selected in transformed image

int i = 0;
string Video = "";
int w = 0, h = 0;
int FramesToSkip = 5;
int TotalFrames = 5736;
vector<float> density_sparse;
vector<float> density_dense;

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

void Output(string filename){
    ofstream outputFile;
    outputFile.open(filename);    
    outputFile << "Density Values\n";
    for(int i = 0; i< density_sparse.size(); i++){
        outputFile << i << ", " << density_sparse[i] << "\n";            
    }    
    outputFile.close();
}


int main(int argc, char* argv[]){
    dst[0] = {472, 52};
    dst[1] = {950, 52};
    dst[2] = {472, 1030};
    dst[3] = {950, 1030};

    if(argc != 2){
        cout << "INVALID INPUT FORMAT\nCORRECT FORMAT: ./M3 <VideoPath>\n";
        return -1;
    }

    auto start = high_resolution_clock::now();
    Video = argv[1];
    // Video = "trafficvideo.mp4";
    

    VideoCapture cap(Video);
    Rect road(472,52, 478, 978);

    if(!cap.isOpened()){ cerr<<"error in opening video file\n"; return 1;}

    char c;

    Mat frame, frame_temp, frame_cropped, frame_warped, matrix, frame_prev, frame_next;
    
    cap>>frame_temp;
    if(frame_temp.empty()) {
        cap.release(); 
        // destroyAllWindows(); 
        return 1;
    }
    
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

    vector<Scalar> colors;
    RNG rng;
    for(int i=0;i<100;i++){
        colors.pb(Scalar(rng.uniform(0,256),rng.uniform(0,256),rng.uniform(0,256)));
    }
    vector<Point2f> p0,p1;
    Mat mask;

    if (i == 4) {
        
        matrix = getPerspectiveTransform(src, dst);
        
        warpPerspective(frame_temp, frame_warped, matrix, frame_temp.size());                
        frame_cropped = frame_warped(road);
        // imshow("Points Selected", img);
        
        frame_prev = frame_cropped;
        goodFeaturesToTrack(frame_prev, p0, 50, 0.3, 7, Mat(), 7, false, 0.04);
        // goodFeaturesToTrack(frame_prev, p0, 200, 0.3, 3, Mat(), 7, );
        mask = Mat::zeros(frame_prev.size(), frame_prev.type());

        destroyWindow("frame_temp");        
        imshow("Cropped Frame", frame_cropped);
        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}

    }
    
    int t = 1;
    destroyWindow("Cropped Frame");
    // cout << "Progress: 0%\n";

    cout << "Processing method 5 of 5 ...\n";
    cout << "Generating Sparse Optical Flow ...\n";

    while(t++){  
        cap>>frame;
        cout << "Progress: " << (int)t*100/TotalFrames << "%    " << '\r' << flush;
        if(t%FramesToSkip!=0){
            if(frame.empty()) break;
            continue;
        }
        // if(t>=50)destroyWindow("Cropped Frame");
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        
        

        if (frame.empty()) break;      
        warpPerspective(frame, frame_warped, matrix, frame.size());
        frame_cropped = frame_warped(road);      
        // imshow("Cropped_frame", frame_cropped);    
        frame_next = frame_cropped.clone();
        
        // Here goes the Optical Flow part vvvv       
        vector<uchar> status; // Status
        vector<float> err;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
        calcOpticalFlowPyrLK(frame_prev, frame_next, p0, p1, status, err, Size(15,15), 2);
        
        vector<Point2f> GoodNew;

        for(uint i = 0; i < p0.size(); i++)
	    {
	        if(status[i] == 1) {
	            GoodNew.push_back(p1[i]);
	            line(mask,p1[i], p0[i], colors[i], 2);
	            // circle(frame_cropped, p1[i], 5, colors[i], -1);
	        }
	    }
        // cout << p0.size() << endl;
        
        // add(frame_cropped, mask, frame_cropped);
        // imshow("Mask", mask);
        

        float threshold = 5;
        float count=0;
        Vec3b pixel_values;
        float value = 0;

        for (int i=0;i<mask.rows;i++){
            for(int j=0;j<mask.cols;j++){
                pixel_values = mask.at<Vec3b>(i,j);
                value = (pixel_values[0] + pixel_values[1] + pixel_values[2])/3;
                if(value >= threshold)count++;
                
            }
        }
        float density_val =  float(count/float(mask.rows*mask.cols));
        density_sparse.pb(density_val);

        // cout << count/float(mask.rows*mask.cols) << "\n";
        mask = Mat::zeros(frame_prev.size(), frame_prev.type());
        // Optical Flow part ^^^
        c = (char)waitKey(25);      
        if(c == 27) break;
        frame_prev = frame_next.clone();

    }
    cout << "Completed 100%\n";

    if(c == 27)return -1;

    Output("Density_Values_M5.csv");

    cap.release();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken: "<< duration.count() << " seconds" << endl;
    // destroyAllWindows();

    start = high_resolution_clock::now();
    

    VideoCapture cap2(Video);
    t = 1;
    vector<float> density_dense;

    cout << "Generating Dense Optical Flow ...\n";

    while(t++){  
        cap2>>frame;
        cout << "Progress: " << (int)t*100/TotalFrames << "%    " << '\r' << flush;
        if(t%FramesToSkip!=0){
            if(frame.empty()) break;
            continue;
        }
        // if(t==50)destroyWindow("Cropped Frame");
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        
        if (frame.empty()) break;      
        warpPerspective(frame, frame_warped, matrix, frame.size());
        frame_cropped = frame_warped(road);      
        // imshow("Cropped_frame", frame_cropped);    
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
        // imshow("frame2", bgr);

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
        density_dense.push_back(density_val);

        // Optical Flow part ^^^
        c = (char)waitKey(25);      
        if(c == 27) break;
        frame_prev = frame_next.clone();

    }

    cout << "Completed 100%\n";
    stop = high_resolution_clock::now();
    duration = duration_cast<seconds>(stop - start);
    cout << "Time taken: "<< duration.count() << " seconds" << endl;

    ofstream outputFile;
    outputFile.open("Density_Values_Baseline_Dynamic.csv");    
    outputFile << "Density Values\n";    
    for(int i = 0; i< density_dense.size(); i++){
        outputFile << i << ", " << density_dense[i] << "\n";            
    }    
    outputFile.close();
    
    cout << "Computing Error ...\n";

    float err=0;
    for(int i=0;i<density_dense.size();i++){
        float val = (density_dense[i] - density_sparse[i])*(density_dense[i] - density_sparse[i]);
        err+=val;
    }
    err/=density_dense.size();
    err = sqrt(err);

    cout << "RMS Error: " << err << "\n";

    cap2.release();

    return 0;
}
