////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: main.cpp
//
// Timeperiod:
//
// 29-Oct-14: Version 1.0: Created
// 29-Oct-14: Version 1.0: Last updated
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define H_RANGE 106

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Load Face cascade (.xml file)
cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eye_cascade;

cv::RNG rng(12345);

inline void face_detection(IplImage* image)
{
   cv::Mat mat_image(image);
   
   // Detect faces
   std::vector<cv::Rect> objects;
   
   std::chrono::time_point<std::chrono::system_clock> start, end;
   
   start = std::chrono::system_clock::now();
   face_cascade.detectMultiScale(mat_image, objects, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(120, 120));
   end = std::chrono::system_clock::now();
   
   // Print the time processing took
   std::chrono::duration<double> elapsed_seconds = end-start;
   std::cout << elapsed_seconds.count() << std::endl;
   
   cv::Mat cropped;
   
   // Print all the objects detected
   for (int i = 0; i < objects.size(); i++)
   {
      cv::rectangle(mat_image, objects[i], cv::Scalar( 255, 0, 0 ));

      cv::Mat object_mat(mat_image, objects[i]);

      object_mat.copyTo(cropped);

      std::vector<cv::Rect> eyes;
      
      start = std::chrono::system_clock::now();
      eye_cascade.detectMultiScale(cropped, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(10, 10));
      end = std::chrono::system_clock::now();
      
      // Print the time processing took
      std::chrono::duration<double> elapsed_seconds = end-start;
      std::cout << elapsed_seconds.count() << std::endl;
      
      std::cout << std::endl;

      // Print all the objects detected
      for (int j = 0; j < eyes.size(); j++)
      {
         cv::Rect eye_rectangle(eyes[j].x + objects[i].x, eyes[j].y + objects[i].y, eyes[j].size().width, eyes[j].size().height);
         
         cv::rectangle(mat_image, eye_rectangle, cv::Scalar(255, 0, 0));

      }
   
      cv::Mat h_mat;
   
      std::vector< std::vector<cv::Point> > contours;
      std::vector<cv::Vec4i> hierarchy;
      std::vector<cv::Mat> color_matrices;
   
      cv::cvtColor(mat_image, mat_image, cv::COLOR_BGR2HSV);
   
      cv::split(mat_image, color_matrices);
      cv::inRange(color_matrices[0], H_RANGE, H_RANGE + 25, color_matrices[0]);
   
      cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
      cv::imshow("Hue", color_matrices[0]);

      cv::waitKey(10);

   }
}

inline bool process_frame(IplImage* frame)
{
   // Look for Face
   face_detection(frame);
   
   // Not finished
   return false;
}

int main()
{
   // Absolute Paths for now
   
   face_cascade.load("haarcascades/haarcascade_frontalface_alt.xml");
   eye_cascade.load("haarcascades/haarcascade_eye.xml");

   video_capture<process_frame, false> input;

   input.capture_sync();

   return 0;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
