#include<vector>

/*PARAMETERS FOR METHOD 1*/
/*For example, SUB_SAMPLE_INCR = 10 and MAX_SUB_SAMPLE = 100 will store utility, runtime for all values x = 10, 20, ... 100*/
/*SUB_SAMPLE_BASE is for specifying x value for baseline*/

#define SUB_SAMPLE_INCR 2
#define SUB_SAMPLE_BASE 1
#define MAX_SUB_SAMPLE 20


/*PARAMETERS FOR METHOD 2*/
/*BASE_RES is the resolution of the source video and image*/
/*enter value of resolutions in vector*/

#define BASE_RES 1080

//do not change the first element of the vector
std::vector<float> resolutions = {BASE_RES, 720, 480, 360, 240, 144};