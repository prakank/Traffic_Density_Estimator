## Queue Density Estimation Runtime-Utility tradeoff analysis
In this subtask of assignment 1, we are performing Runtime-Utility tradeoff analysis on a given video of a traffic intersection.
In the given directory, we have done methods 3, 4 and 5. In method 3 and 4, we vary the number of threads, and in method 5, we compare Sparse Optical Flow with Dense Optical Flow.

### Inputting Parameters

_Method 3 and Method 4:_ The user can input the number of threads required on the console itself. User can change the speed of processing the video (FPS) and total frames of the video in **dependencies_3_4_5.hpp** file. For example, user can change "FramesToSkip" variable in the file.

_Method 5:_ Similar to Method 3 and 4, user can change the speed of processing the video (FPS) and total number of frames.

### Compilation and Execution
For compiling **method3.cpp**, **method4.cpp** and **method5.cpp**, use `make` or ` make all` in the terminal. 

For execution of method3 binary run(the video filename **trafficvideo.mp4** and number of threads **4** can be different)
```sh
./method3 trafficvideo.mp4  4
```

Similarly for execution of method4 binary run
```sh
./method4 trafficvideo.mp4 4
```
For execution of method5 binary run
```sh
./method5 trafficvideo.mp4
```

### Plotting graphs
The images of plots are stored in **Method3_utility.png**, **Method4_utility.png** and **Method_5_Sparse_vs_Dense.png**

To get the plot **Error_Utility_3.csv** and **Error_Utility_3.csv**, run
```sh
make plot3_utility
make plot4_utility
```
To get the comparison plots, run
```sh
make plot3
make plot4
make plot5
```

### To remove created files/binaries/images
```sh
make clean
```
