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

// Load Face cascade (.xml file)
cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eye_cascade;

inline void face_detection(IplImage* image)
{
   cv::Mat mat_image(image);
   
   // Detect faces
   std::vector<cv::Rect> objects;
   
   std::chrono::time_point<std::chrono::system_clock> start, end;
   
   start = std::chrono::system_clock::now();
   eye_cascade.detectMultiScale(mat_image, objects, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(75, 75));
   end = std::chrono::system_clock::now();
   
   // Print the time processing took
   std::chrono::duration<double> elapsed_seconds = end-start;
   std::cout << elapsed_seconds.count() << std::endl;
   
   cv::Mat cropped;
   
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