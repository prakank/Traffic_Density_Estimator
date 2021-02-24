## Camera angle correction and frame cropping

The given assignment is intended to output a transformed perspective image and its cropped version given an original image. The user selects 4 points and the corresponding quadrangle is projected onto a rectangular frame of fixed dimensions using OpenCV functions.

### How to use?

* Compile the code.cpp file using the command
```sh
make all
```
* To execute on target image <imagefile.xyz>
```sh
./code imagefile.xyz
```
* To remove executable binary and generated images
```sh
make clean
```
* For help menu
```sh
make help
```

#### Note
4 points need to be selected for generating the corresponding transformed and cropped images. For generating an upright image, select the 4 points in the order corresponding to top left, top-right, bottom left, bottom right positions of the rectangle respectively. This generates a perspective transformed version of the image, click on any key to generate the cropped image, and click once more to exit.
