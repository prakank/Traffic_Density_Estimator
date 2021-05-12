## Queue Density Estimation utility-runtime tradeoff analysis
In this subtask of assignment 1, we are performing utility-runtime tradeoff analysis on a given video of a traffic intersection.
In the given directory, we have done methods 1 and 2. In method 1, we vary the parameter x, i.e. interval of sub-sampling of frames. In method2, we vary the resolution of the video frames

### Inputting Parameters

_Method 1:_ The user can input parameters by modifying the **dependencies_1_2.hpp** file. For example, by setting `SUB_SAMPLE_BASE` to be 1, we are fixing the baseline to have parameter x = 1. If `SUB_SAMPLE_INCR` is 5 and `MAX_SUB_SAMPLE` is 20, then the runtime and utility values are stored in **out1.csv** file for parameters 5, 10, 15, 20. These can then be plotted as mentioned below.

_Method 2:_ The user can input parameters by modifying the **dependencies_1_2.hpp** file. In this case, the user can define `BASE_RES` of the video which will act as a baseline resolution against which error is calculated. The resolution values are the Y resolution values. Since, aspect ratio is fixed to 16:9, no X resolution values are required. In the `resolutions` vector, the user can input all Y resolution values for which runtime and utility are stored in **out2.csv**. These can then be plotted as mentioned below.

### Compilation and Execution
For compiling **method1.cpp** and **method2.cpp**, use `make` or ` make all` in the terminal. 

For execution of method1 binary run(the video filename **trafficvideo.mp4** and image filename **Empty_frame.jpg** can be different)
```sh
./method1 trafficvideo.mp4  Empty_frame.jpg
```

Similarly for execution of method2 binary run
```sh
./method2 trafficvideo.mp4 Empty_frane,jpg
```

### Plotting graphs
The images of plots are stored in **method1.png** and **method2.png**
To plot **out1.csv** and **out2.csv**, run
```sh
make plot
```
or
```sh
python3 graph_plot.py
```

### To remove created files/binaries/images
```sh
make clean
```
