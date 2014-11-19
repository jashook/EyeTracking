////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook | Edited by Andrew Blanchard
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
// Implements eyeLike library (sort of..) - found: http://thume.ca/projects/2012/11/04/simple-accurate-eye-center-tracking-in-opencv/
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if _WIN32

// Windows

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\highgui\highgui_c.h>
#include <opencv2\imgproc\imgproc_c.h>
#include <opencv2\imgproc\imgproc.hpp>

#else

// Unix Based System

#include <opencv/objdetect/objdetect.hpp>
#include <opencv/highgui/highgui.hpp>
#include <opencv/highgui/highgui_c.h>
#include <opencv/imgproc/imgproc_c.h>
#include <opencv/imgproc/imgproc.hpp>

#endif

#include <iostream>
#include <cstdio>
#include <vector>
#include <chrono>

#include "video_capture.hpp"
#include "eyeLike\src\findEyeCenter.h"
#include "eyeLike\src\constants.h"

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
   face_cascade.detectMultiScale(mat_image, objects, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));
   end = std::chrono::system_clock::now();

   //I got lazy. so I added the center of eye detection here. I'll fix the timing later. Probably.. - Andrew
   //*****************Added code starts here*********************************

   //if no faces are detected, return
   if (objects.size() == 0)
	   return;

   //-- Find eye regions
				//side note: this is blazing fast compared to using open cv's eye detection, but it uses magic numbers.. so iunno.
   cv::Rect face = objects[0];
   cv::Mat faceROI = mat_image(face);
   cv::Mat debugFace = faceROI;
   int eye_region_width = face.width * (kEyePercentWidth / 100.0);
   int eye_region_height = face.width * (kEyePercentHeight / 100.0);
   int eye_region_top = face.height * (kEyePercentTop / 100.0);
   cv::Rect leftEyeRegion(face.width * (kEyePercentSide / 100.0),
	   eye_region_top, eye_region_width, eye_region_height);
   cv::Rect rightEyeRegion(face.width - eye_region_width - face.width * (kEyePercentSide / 100.0),
	   eye_region_top, eye_region_width, eye_region_height);

   //-- Draw eye regions (left: green, right: red)
   cv::rectangle(faceROI, leftEyeRegion, cv::Scalar(0,255,0));
   cv::rectangle(faceROI, rightEyeRegion, cv::Scalar(0, 0, 255));


   ////-- Find Eye Centers
   //cv::Point leftPupil = findEyeCenter(faceROI, leftEyeRegion, "Left Eye");
   //cv::Point rightPupil = findEyeCenter(faceROI, rightEyeRegion, "Right Eye");

   //// change eye centers to face coordinates
   //rightPupil.x += rightEyeRegion.x;
   //rightPupil.y += rightEyeRegion.y;
   //leftPupil.x += leftEyeRegion.x;
   //leftPupil.y += leftEyeRegion.y;

   //// draw eye centers
   //circle(debugFace, rightPupil, 3, 1234);
   //circle(debugFace, leftPupil, 3, 1234);

   //***************Added code ends here***********************
   
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