#include "functions.hpp"

int main(int argc, char* argv[]){

    char c;
    string s = argv[1];

    Mat frame, frame_temp, frame_next, frame_prev, frame_cropped, frame_warped, matrix, ref_frame, ref_frame_cropped, ref_frame_warped, diff_frame, diff_frame_thresh, blur_dst;

    /*code for generating cropped frame and Homography_matrix*/
    dst[0] = {472, 52};
    dst[1] = {950, 52};
    dst[2] = {472, 1030};
    dst[3] = {950, 1030};

    //string path = argv[1];
    

    VideoCapture cap(s);
    Rect road(472,52, 478, 978);

    if(!cap.isOpened()) {cout<<"error in opening video file"<<argv[1]<<"\n";return 1;}

    int n_frames = int(cap.get(CAP_PROP_FRAME_COUNT));

    
    
    ref_frame = imread(argv[2]);
    if(ref_frame.empty()){cout<<"Error in loading image "<<argv[2]<<"\n"; return 1;}
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
        
        frame_prev = frame_cropped;
        
        // imshow("Points Selected", img);
        
        destroyWindow("frame_temp");
        
        /*imshow("Cropped Frame", frame_cropped);*/
        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}

    }

    cap.release();
    /*end of code block*/

    bool escaped = false;
    
    int sub_sampling_param = 0;
    bool base_done = false; 
    int progress = 0;

    /*code for processing video frames*/
    
    cout<<"Processing task 1 ...\n";

    while(sub_sampling_param < MAX_SUB_SAMPLE)
    {
        VideoCapture cap2(s);
        if(!base_done) sub_sampling_param = SUB_SAMPLE_BASE;
        else sub_sampling_param +=SUB_SAMPLE_INCR;
        float prev_queue_density = 0;
        float error, utility = 0;

        int t = 1;
        waitKey(100);
        auto start = high_resolution_clock::now();
        while(t++){
            progress = t*100/(n_frames);
            if(!base_done) cout<<"Initializing "<<progress<<"%    "<<'\r'<<std::flush;
            else cout<<"Cycle "<<sub_sampling_param / SUB_SAMPLE_INCR<<" of "<<(MAX_SUB_SAMPLE/SUB_SAMPLE_INCR)<<": "<<progress<<"%    "<<'\r'<<std::flush;
            //cout<<sub_sampling_param<<"\t\t"<<t<<'\r'<<std::flush;
            
            cap2>>frame;
            //pBackSub->apply(frame, fgMask);
            
            if(t % sub_sampling_param!=0){
                if(frame.empty()) break;
                if(!base_done) base_queue_density.push_back(prev_queue_density);
                else{
                    error =  pow((prev_queue_density - base_queue_density[t-2]), 2);
                    utility = utility + error;
                }
                //temp_queue_density.push_back(prev_queue_density);
                continue;
            }
        
            if (frame.empty()) break;
            cvtColor(frame, frame, COLOR_BGR2GRAY);
        
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

            
            prev_queue_density = sum/(frame_cropped.size().height*frame_cropped.size().width);

            if(!base_done) base_queue_density.push_back(prev_queue_density);
            else{
                error =  pow((prev_queue_density - base_queue_density[t-2]), 2);
                utility = utility + error;
            }
            //temp_queue_density.push_back(prev_queue_density);
            
        
        
        
        }

        cap2.release();

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        if(!base_done) {base_done = true; sub_sampling_param = 0;}

        if(t > 1)
        {
            utility = utility/(t-1);
            utility = 1/utility;
            utility = log(utility);
            pair<float, float> p = make_pair(utility, duration.count());
            param_utility_map.insert(pair<int, pair<float, float>>(sub_sampling_param, p));
        }

        

        

    }

    if(!escaped)
    {
        string filename = "out1.csv";
        ofstream outputFile(filename);
        outputFile<<"Utility,Runtime(in milliseconds),Parameter x(Interval of sub-sampling)\n";
        for(auto it = param_utility_map.begin(); it!= param_utility_map.end(); it++){
            outputFile<<it->second.first<<","<<it->second.second<<","<<it->first<<"\n";
        }
    
    

    outputFile.close();

    }
    

    
    if(!escaped) cout<<"Completed 100%\n";

    cap.release();

    destroyAllWindows();

}