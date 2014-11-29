////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook | Modified by Andrew Blanchard
//
// Module: main.cpp
//
// Timeperiod:
//
// 29-Oct-14: Version 1.0: Created
// 18-Nov-14: Version 1.0: Last updated
//
// Notes:
//
// Uses opencv
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if _WIN32

// Windows

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\highgui\highgui_c.h>
#include <opencv2\imgproc\imgproc_c.h>
#include <opencv2\imgproc\imgproc.hpp>

#else

// Unix Based System

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#endif

#include <iostream>
#include <cstdio>
#include <vector>
#include <chrono>

#include "video_capture.hpp"
#include "constants.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Load Face cascade (.xml file)
cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eye_cascade;

inline void face_detection(IplImage* image)
{
   //debug flag
   bool displayTemps = false;
   if (displayTemps)
   {
      cv::namedWindow("debug_gray", CV_WINDOW_AUTOSIZE);
      cv::namedWindow("debug_color", CV_WINDOW_AUTOSIZE);
      cv::namedWindow("debug_blur", CV_WINDOW_AUTOSIZE);
   }
   else
      cv::namedWindow("debug_blur", CV_WINDOW_AUTOSIZE);

   cv::Mat mat_image(image);
   cv::Mat mat_gray = mat_image;

   //convert to gray scale
   cvtColor(mat_image, mat_gray, CV_BGR2GRAY);
   if (displayTemps)
   {
      cv::imshow("debug_gray", mat_gray);
      cv::imshow("debug_color", mat_image);
   }

   // Detect faces
   std::vector<cv::Rect> objects;

   std::chrono::time_point<std::chrono::system_clock> start, end;

   start = std::chrono::system_clock::now();
   face_cascade.detectMultiScale(mat_gray, objects, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(75, 75));
   end = std::chrono::system_clock::now();

   // Print the time processing took
   std::chrono::duration<double> elapsed_seconds = end - start;
   std::cout << elapsed_seconds.count() << std::endl;

   //check to see that faces were found - if not, return
   if (objects.size() < 1)
      return;
   
   //-- Find eye regions (using magic numbers from constants.h)
   cv::Rect face = objects[0];
   cv::Mat faceROI_gray = mat_gray(face);
   cv::Mat faceROI = mat_image(face);
   int eye_region_width = face.width * (kEyePercentWidth / 100.0);
   int eye_region_height = face.width * (kEyePercentHeight / 100.0);
   int eye_region_top = face.height * (kEyePercentTop / 100.0);
   cv::Rect leftEyeRegion(face.width * (kEyePercentSide / 100.0),
      eye_region_top, eye_region_width, eye_region_height);
   cv::Rect rightEyeRegion(face.width - eye_region_width - face.width * (kEyePercentSide / 100.0),
      eye_region_top, eye_region_width, eye_region_height);

   //make a mat out of the left eye
   cv::Mat lEyeROI = faceROI_gray(leftEyeRegion);
   cv::Mat lEyeROI_color = faceROI(leftEyeRegion);
   if (displayTemps)
      cv::imshow("debug_color", lEyeROI_color);

   //apply median filter
   cv::medianBlur(lEyeROI, lEyeROI, 7);
   //display result
   cv::imshow("debug_blur", lEyeROI);

   //apply hough transform
   std::vector<cv::Vec3f> circles;
   cv::HoughCircles(lEyeROI, circles, CV_HOUGH_GRADIENT,
      5,       //accumulator resolution
      100,      //minimum distance between two circles
      100,     //canny high threshhold
      60,      //minimum number of votes
      0, 200); //min and max radius

   //draw center of detected circles
   for (size_t i = 0; i < circles.size(); i++)
   {
      //draw points
      cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      cv::circle(lEyeROI_color, center, 3, cv::Scalar(0, 255, 0));
      cv::circle(lEyeROI, center, 3, cv::Scalar(0, 0, 255));
      cv::imshow("debug_blur", lEyeROI);
      int x = 3;
   }

   //-- Draw eye regions (left: green, right: red)
   cv::rectangle(faceROI, leftEyeRegion, cv::Scalar(0, 255, 0));
   cv::rectangle(faceROI, rightEyeRegion, cv::Scalar(0, 0, 255));
   
   // Print all the objects detected
   for (int i = 0; i < objects.size(); i++)
   {
      cv::rectangle(mat_image, objects[i], cv::Scalar( 255, 0, 0 ));
   }
}

inline bool process_frame(IplImage* frame)
{
   // process the frame here
   
   // Look for Face
   face_detection(frame);
   
   // Not finished
   return false;
}

inline void hough_test()
{
   using namespace cv;

   //load image (test eye image located in resources)
   Mat src, src_gray;
   //read image
   src = imread("eye.jpg", CV_LOAD_IMAGE_COLOR);
   //display image
   namedWindow("color", WINDOW_AUTOSIZE);
   imshow("color", src);

   //proceess image
   //convert to gray
   cvtColor(src, src_gray, CV_BGR2GRAY);
   //display gray
   namedWindow("gray");
   imshow("gray", src_gray);
   //blur (median filter cuz fast)
   medianBlur(src_gray, src_gray, 21);
   //display blur
   namedWindow("gray_blur");
   imshow("gray_blur", src_gray);
   //hough transform
   vector<Vec3f> circles;
   HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 2, src_gray.rows / 8, 100, 100, 0, 500);
   //draw circles
   for (size_t i = 0; i < circles.size(); i++)
   {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      //draw center
      circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
      //draw seg circle
      circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
   }
   imshow("color", src);

   //pause before return
   waitKey(0);
   int x = 3; //used with a breakpoint
}

int main()
{
   // Absolute Paths for now
   
   #ifdef _WIN32
      face_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
      eye_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye.xml");

   #else
      face_cascade.load("/Users/jarret/Downloads/haarcascade_frontalface_alt.xml");
      eye_cascade.load("/Users/jarret/Downloads/haarcascade_eye.xml");
   
   #endif
   
   video_capture<process_frame, true> input;

   input.capture_sync();

   //hough_test();
   return 0;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////