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

int H_RANGE = 106;

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
   face_cascade.detectMultiScale(mat_image, objects, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(100, 100));
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
      eye_cascade.detectMultiScale(cropped, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(1, 1));
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
      
   }
}

inline bool process_frame(IplImage* frame)
{
   // Look for Face
   face_detection(frame);
   
   /*cv::Mat mat_image(frame);
   cv::Mat h_mat;
   
   std::vector< std::vector<cv::Point> > contours;
   std::vector<cv::Vec4i> hierarchy;
   std::vector<cv::Mat> color_matrices;
   
   cv::cvtColor(mat_image, mat_image, cv::COLOR_BGR2HSV);
   
   cv::split(mat_image, color_matrices);
   cv::inRange(color_matrices[0], H_RANGE, H_RANGE + 10, color_matrices[0]);
   
   //color_matrices[2].copyTo(h_mat);
   
   //cv::findContours(h_mat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
   
   cv::Mat drawing = cv::Mat::zeros(h_mat.size(),CV_8UC3 );
   for( int i = 0; i< contours.size(); i++ )
   {
      cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      cv::drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
   }
   
   int key = cv::waitKey(20);
   
   if (key == (int)'u')
   {
      H_RANGE += 1;
   }
   
   else if (key == (int)'d')
   {
      H_RANGE -= 1;
   }
   
   std::cout << H_RANGE << std::endl;
   
   cv::imshow("Hue", color_matrices[0]);
   */
   // Not finished
   return false;
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

   return 0;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////