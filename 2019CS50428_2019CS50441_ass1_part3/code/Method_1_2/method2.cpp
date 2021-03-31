#include "functions.hpp"
#define xx cout<<"xxx\n";

int main(int argc, char* argv[]){
    

    Mat frame, frame2, frame_temp, frame_temp2, ref_frame2, frame_next, frame_prev, frame_cropped, frame_warped, matrix, ref_frame, ref_frame_cropped, ref_frame_warped, diff_frame, diff_frame_thresh, blur_dst;
    string s = argv[1];
    VideoCapture cap(s);
    char c;

    if(!cap.isOpened()) {cout<<"error in opening video file "<<argv[1]<<"\n";return 1;}
    int n_frames = int(cap.get(CAP_PROP_FRAME_COUNT));

    ref_frame = imread(argv[2]);
    if(ref_frame.empty()){cout<<"Error in loading image "<<argv[2]<<"\n"; return 1;}
    cvtColor(ref_frame, ref_frame, COLOR_BGR2GRAY);


    //cout<<cap.get(3)<<" "<<cap.get(4)<<endl;

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

    if(i == 4){
        destroyWindow("frame_temp");

        c = (char)waitKey(25);
        if(c == 27) {cap.release(); destroyAllWindows(); return 0;}
    }
    else {cout<<"error while selecting points"<<endl; return 1;}

    cap.release();



    
    bool base_done = false; 
    bool escaped = false;
    int progress = 0;
    int cycle = -1;


    cout<<"Processing task 2 ...\n";
    for(auto res:resolutions){
        cycle++;
        

        /*code for generating cropped frame and Homography_matrix*/
        dst[0] = {472 * res/BASE_RES, 52 * res/BASE_RES};
        dst[1] = {950 * res/BASE_RES, 52 * res/BASE_RES};
        dst[2] = {472 * res/BASE_RES, 1030 * res/BASE_RES};
        dst[3] = {950 * res/BASE_RES, 1030 * res/BASE_RES};
        

        Rect road(472 * res/BASE_RES, 52 * res/BASE_RES, 478 * res/BASE_RES, 978 * res/BASE_RES);

        Point2f src2[4];
        for(int k = 0;k < 4; k++){
            src2[k] = src[k] * res/BASE_RES;
        }

        matrix = getPerspectiveTransform(src2, dst);

        resize(frame_temp, frame_temp, Size(res * 16/9, res));

        resize(ref_frame, ref_frame, Size(res * 16/9, res));
        
        warpPerspective(frame_temp, frame_warped, matrix, Size(res * 16/9, res));
        
        warpPerspective(ref_frame, ref_frame_warped, matrix, Size(res * 16/9, res));
        
        ref_frame_cropped = ref_frame_warped(road);
        
        frame_cropped = frame_warped(road);
        
        frame_prev = frame_cropped;
        
        /*end of code block*/
        
        int sub_sampling_param = 0;

        /*code for processing video frames*/

        VideoCapture cap2(s);
        if(!cap2.isOpened()) {cout<<"Error opening video file"; return 1;}
        float prev_queue_density;
        float error, utility = 0;

        int t = 1;

        auto start = high_resolution_clock::now();
        while(t++){
            progress = t*100/(n_frames);
            if(!base_done) cout<<"Initializing "<<progress<<"%    "<<'\r'<<std::flush;
            else cout<<"Cycle "<<cycle<<" of "<<resolutions.size() - 1<<": "<<progress<<"%    "<<'\r'<<std::flush;
            //cout<<sub_sampling_param<<"\t\t"<<t<<'\r'<<std::flush;
            
            cap2>>frame;
            //pBackSub->apply(frame, fgMask);
        
            if (frame.empty()) break;
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            resize(frame, frame, Size(res * 16/9, res));
        
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

        if(!base_done) {base_done = true;}

        if(t > 1)
        {
            utility = utility/(t-1);
            utility = 1/utility;
            utility = log(utility);
            pair<float, float> p = make_pair(utility, duration.count());
            param_utility_map.insert(pair<float, pair<float, float>>(res, p));
        }

            

        /*previous while end*/

    }

    if(!escaped)
        {
            string filename = "out2.csv";
            ofstream outputFile(filename);
            outputFile<<"Utility,Runtime(in milliseconds),Parameter res(Resolution in p)\n";
            for(auto it = param_utility_map.begin(); it!= param_utility_map.end(); it++){
                outputFile<<it->second.first<<","<<it->second.second<<","<<it->first<<"\n";
            }
        
        

            outputFile.close();

        }

        
        if(!escaped) cout<<"Completed Task 2 100%\n";

    cap.release();

    destroyAllWindows();

}