#include "functions_3_4_5.hpp"

int NumberOfThreads = 1;
int TotalTimeTaken1;
int TotalTimeTaken2;
int Progress = 0;
bool Generate_Baseline = true;
char Esc;
bool Escaped = false;

vector<float> fin;
vector<float> v1;
vector<float> v2;
vector<Combined_short> v;

string Video = "";
Rect road(472,52, 478, 978);
Mat ref_frame, ref_frame_cropped, ref_frame_warped, matrix;

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

        Combined_short cd;
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

    sort(v.begin(), v.end(),Comparator_short);

    string filename = "Density_Values_M4.csv";
    ofstream outputFile(filename);    
    outputFile << "FrameNumber,Density Values\n";
    outputFile << NumberOfThreads << ", " << to_string(TotalTimeTaken1) << "_" << to_string(TotalTimeTaken2) << "\n";
    for(int i = 0; i< v.size(); i++){
        outputFile << i+1 << ", " << v[i].Density << "\n";
        fin.push_back(v[i].Density);
    }    
    outputFile.close();
}

int main(int argc, char* argv[]){
        
    if(argc < 3 || argc > 4){
        cout << "INVALID INPUT FORMAT\nCORRECT FORMAT: ./method3 <VideoPath> <NumberOfThreads>\n";
        return -1;
    }

    if(argc==4)Generate_Baseline = argv[3];
    
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

    if(Generate_Baseline){
        ofstream file("Source.csv");
        file << src[0].x << ", " << src[0].y << "\n";
        file << src[1].x << ", " << src[1].y << "\n";
        file << src[2].x << ", " << src[2].y << "\n";
        file << src[3].x << ", " << src[3].y << "\n";
        file.close()        
    }
    
    cout << "\nProcessing method 4 of 5 ...\n";
    cout << "Generating Queue Density with Number of Threads = " << NumberOfThreads <<" ...\n";
    auto start = high_resolution_clock::now();
    vector <thread> threads;
    
    for(int p=0; p < NumberOfThreads; p++){
        threads.push_back(thread(BackgroundSubtraction, p+1));
    }

    for(int i=0;i<NumberOfThreads; i++)threads[i].join();
    cout << "Completed 100%\n";
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    TotalTimeTaken1 = duration.count();
    cout << "Time taken: "<< duration.count() << " seconds" << endl;
    // cout << "Number of threads: " << NumberOfThreads << endl;
    

    // Below Method is for generating Baseline Queue Values
    

    start = high_resolution_clock::now();
    VideoCapture cap2(Video);
    cout<<"\nGenerating Baseline Queue Density ...\n";        
    bool escaped = false;
    int t = 1;
    while(t++){

        if(!Generate_Baseline)break;

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

    Output(); // Write output  of Method 4 in a csv file
    // Called this function later so as to obtain the value of TotalTimeTaken2

    if(Generate_Baseline){
        ofstream outputFile;
        outputFile.open("Density_Values_Baseline_Queue.csv");    
        outputFile << "Density Values\n";
        for(int i = 0; i< v1.size(); i++){
            outputFile << i << ", " << v1[i] << "\n";            
        }    
        outputFile.close();
    }
    else{
        ifstream outputFile("Density_Values_Baseline_Queue.csv");
        string out;
        int skip=0;
        while(getline(outputFile,out)){
            if(skip==0){skip++;continue;}
            vector<string> temp;
            boost::split(temp,out,boost::is_any_of(","));
            v1.push_back(stof(temp[1]));
        }
    }
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
    cout << "Utility (log[1/error]):" << utility << "\n\n";

    string Error = "4, " + to_string(NumberOfThreads) + ", " + to_string(TotalTimeTaken1) + ", " + to_string(err) + ", " + to_string(utility) + "\n";
    string Baseline_Error = "0, 0, "  + to_string(TotalTimeTaken2) + ", 0, 0\n";

    string out;
    ifstream read("Error_Utility_4.csv");
    if(read){
        vector<string> file; 
        while(getline(read, out))file.push_back(out);
        read.close();
        ofstream outputFile("Error_Utility_4.csv");
        for(int i=0;i<file.size();i++){
            outputFile << file[i] << "\n";
        }
        outputFile << Error;
        outputFile << Baseline_Error;
        outputFile.close();
    }
    else{
        ofstream outputFile("Error_Utility_4.csv");
        cout << "Method, Threads, Runtime, Error, Utility\n";
        outputFile << Error;
        outputFile << Baseline_Error;
        outputFile.close();
    }

}