// Include files for required libraries
#include <stdio.h>
#include "opencv_aee.hpp"
#include "main.hpp" // You can use this file for declaring defined values and functions
#include "pi2c.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/detail/distortion_model.hpp"
#include "opencv2/imgproc/hal/hal.hpp"
#include "opencv2/imgproc/hal/interface.h"
using namespace cv;
using namespace std;
// Example setup, replace with actual setup steps for your library
void setupI2C(int address)
{
//pi2c_begin(address); // Assuming this function exists
}
void setup(void)
{
setupCamera(320, 240); // Enable the camera for OpenCV
}
// Function to threshold the image for a specific color range
Mat thresholdForColor(const Mat& hsvImage, const Scalar& lowerBound, const Scalar&
upperBound)
{
Mat mask;
inRange(hsvImage, lowerBound, upperBound, mask);
return mask;
}
// Function to apply morphological operations to clean up the mask
Mat cleanMask(const Mat& mask)
{
Mat cleanedMask;
Mat kernelOpen = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
morphologyEx(mask, cleanedMask, MORPH_OPEN, kernelOpen);
Mat kernelClose = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
morphologyEx(cleanedMask, cleanedMask, MORPH_CLOSE, kernelClose);
return cleanedMask;
}
// Function to check if a color is present in the image
bool isColorPresent(const Mat& mask)
{
int nonZeroCount = countNonZero(mask);
// Threshold might need adjustment based on the image size and expected object size
return nonZeroCount > 100;
}
// Updated findAndDrawContours function with area and center point calculation
void findAndDrawContours(const Mat& mask, Mat& frame, const Scalar& color)
{
vector<vector<Point> > contours;
findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
double maxArea = 0.0;
int maxAreaContourIndex = -1;
for (int i = 0; i < contours.size(); i++)
{
double area = contourArea(contours[i]);
if (area > maxArea)
{
maxArea = area;
maxAreaContourIndex = i;
}
}
if (maxAreaContourIndex != -1)
{
drawContours(frame, contours, maxAreaContourIndex, color, 2);
// Calculate the center of the contour
Point regionCentre = findContourCentre(contours[maxAreaContourIndex]);
// Optionally draw the center point
circle(frame, regionCentre, 5, Scalar(0, 0, 255), -1);
printf("Contour center: x = %dpx, y = %dpx\n", regionCentre.x, regionCentre.y);
int y;
if (regionCentre.x >= 0 && regionCentre.x < 40)
{
y = 0;
}
else if (regionCentre.x >= 40 && regionCentre.x < 80)
{
y = 1;
}
else if (regionCentre.x >= 80 && regionCentre.x < 120)
{
y = 2;
}
else if (regionCentre.x >= 120 && regionCentre.x < 160)
{
y = 3;
}
else if (regionCentre.x >= 160 && regionCentre.x < 200)
{
y = 4;
}
else if (regionCentre.x >= 200 && regionCentre.x < 240)
{
y = 5;
}
else if (regionCentre.x >= 240 && regionCentre.x < 280)
{
y = 6;
}
else // Assuming x is between 280 and 320
{
y = 7;
}
Pi2c arduino(4); //Create a new object "arduino" using address "0x04"
char receive[16]; //Create a buffer of char (single bytes) for the data
//Send an 16 bit integer
arduino.i2cWriteArduinoInt(y);
}
}
int main( int argc, char** argv )
{
setup(); // Call a setup function to prepare IO and devices
cv::namedWindow("Photo"); // Create a GUI window called photo
Mat contour;
while(1) // Main loop to perform image processing
{

Mat frame;

while(frame.empty())
frame = captureFrame();// Capture the frame from the RPI camera and store in frame variable


matrix variable
rotate(frame, frame, ROTATE_180); //rotate image 180 degrees
// Convert framed image into colour space
Mat hsvImage;
cvtColor(frame, hsvImage, COLOR_BGR2HSV);
//Scalar lowerBlack(0,0,0),upperBlack(179,255,60)


// Define HSV ranges for different colors
Scalar lowerBlue(99, 115, 86), upperBlue(107, 255, 255);
Scalar lowerGreen(58, 103, 49), upperGreen(92, 255, 255);
Scalar lowerRed(120, 70, 50), upperRed(179, 255, 255);
Scalar lowerYellow(29, 130, 0), upperYellow(93, 255, 255);
//Scalar lowerBlack(24,0,0),upperBlack(140,160,65)
// Threshold and clean masks for each color
Mat greenMask = cleanMask(thresholdForColor(hsvImage, lowerGreen, upperGreen));
Mat blueMask = cleanMask(thresholdForColor(hsvImage, lowerBlue, upperBlue));
Mat redMask = cleanMask(thresholdForColor(hsvImage, lowerRed, upperRed));
Mat YellowMask = cleanMask(thresholdForColor(hsvImage, lowerYellow, upperYellow));
Mat blackMask = cleanMask(thresholdForColor(hsvImage, lowerBlack, upperBlack));
// Check for the presence of each color
if (isColorPresent(blackMask))
{
printf("Black color");
findAndDrawContours(blackMask, frame, Scalar(255, 0, 0));
imshow("Black photo", blackMask);
}
if (isColorPresent(blueMask))
{
printf("Blue color");
findAndDrawContours(blueMask, frame, Scalar(255, 0, 0));
imshow("Blue photo", blueMask);
}
if (isColorPresent(greenMask))
{
printf( "Green color" );
findAndDrawContours(greenMask, frame, Scalar(255, 0, 0));
imshow("Green photo", greenMask);
}
if (isColorPresent(redMask))
{
printf( "Red color");
imshow("Red photo", redMask);
findAndDrawContours(redMask, frame, Scalar(255, 0, 0));
}
if (isColorPresent(yellowMask))
{
printf( "yellow color");
imshow("yellow photo", YellowMask);
findAndDrawContours(YellowMask, frame, Scalar(255, 0, 0));
}


// Find and draw contours for each color
cv::imshow("Photo", frame); //Display the image in the window
int key = cv::waitKey(1); // Wait 1ms for a keypress (required to update windows)
key = (key==255) ? -1 : key; // Check if the ESC key has been pressed
if (key == 27)
break;
}
closeCV(); // Disable the camera and close any windows
return 0;
}
