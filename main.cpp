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
#include <opencv2/imgproc/imgproc.hpp>

#endif

#include <iostream>
#include <cstdio>
#include <vector>
#include <chrono>

#include "constants.hpp"
#include "find_eye_center.hpp"
#include "video_capture.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if 0

#define DEBUG_FLAG

#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Load Face cascade (.xml file)
cv::CascadeClassifier face_cascade;

inline void face_detection(cv::Mat& image)
{
   #ifdef DEBUG_FLAG
      cv::namedWindow("debug_gray", CV_WINDOW_AUTOSIZE);
      cv::namedWindow("debug_color", CV_WINDOW_AUTOSIZE);
      cv::namedWindow("debug_blur", CV_WINDOW_AUTOSIZE);
   #endif
  
   cv::Mat mat_gray = image;

   // Convert to gray scale
   cvtColor(image, mat_gray, CV_BGR2GRAY);
   
   #ifdef DEBUG_FLAG
      cv::imshow("debug_gray", mat_gray);
      cv::imshow("debug_color", image);
   #endif

   // Detect faces
   std::vector<cv::Rect> faces;

	//dynamically scale min object size by the width of the image (hueristically determined to be imgWidth / 4)  ((hueristically is a cool word for made up))
	int minObjectSize = image.cols / 4;
   face_cascade.detectMultiScale(mat_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(minObjectSize, minObjectSize));

   // Check to see that faces were found - if not, return
   if (faces.size() < 1)
   {
      return;
   }
      
   // Find eye regions (numbers included from constants.h) (only use the first face)
   cv::Rect face = faces[0];

   cv::Mat face_roi_gray = mat_gray(face);
   cv::Mat face_roi = image(face);
   
   int eye_region_width = face.width * (kEyePercentWidth / 100.0);
   int eye_region_height = face.width * (kEyePercentHeight / 100.0);
   int eye_region_top = face.height * (kEyePercentTop / 100.0);
   
   cv::Rect left_eye_region(face.width * (kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);

   cv::Rect right_eye_region(face.width - eye_region_width - face.width * (kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);

   /////////////////////////////////////////////////////////////////////////////
   // EYE LIKE
   /////////////////////////////////////////////////////////////////////////////

   //-- Find Eye Centers
   cv::Point leftPupil = findEyeCenter(face_roi_gray, left_eye_region, "Left Eye");
   cv::Point rightPupil = findEyeCenter(face_roi_gray, right_eye_region, "Right Eye");

   // change eye centers to face coordinates
   rightPupil.x += right_eye_region.x;
   rightPupil.y += right_eye_region.y;
   leftPupil.x += left_eye_region.x;
   leftPupil.y += left_eye_region.y;

   // draw eye centers
   circle(face_roi, rightPupil, 3, cv::Scalar(0, 255, 0));
   circle(face_roi, leftPupil, 3, cv::Scalar(0, 255, 0));

   /////////////////////////////////////////////////////////////////////////////
   // Draw eye regions (left: green, right: red)
   /////////////////////////////////////////////////////////////////////////////
   
   cv::rectangle(face_roi, left_eye_region, cv::Scalar(0, 255, 0));
   cv::rectangle(face_roi, right_eye_region, cv::Scalar(0, 0, 255));
   
   // Print all the objects detected
   for (int i = 0; i < faces.size(); i++)
   {
      cv::rectangle(image, faces[i], cv::Scalar( 255, 0, 0 ));
   }
}

inline bool process_frame(cv::Mat& frame)
{
   // process the frame here

	//define timing vars
	std::chrono::time_point<std::chrono::system_clock> start, end;

	start = std::chrono::system_clock::now();
	// Look for Face
	face_detection(frame);
	end = std::chrono::system_clock::now();

	// Print the time processing took
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "fps: " << 1/elapsed_seconds.count() << std::endl;
   
   
   // Not finished
   return false;
}

int main()
{
   // Absolute Paths for now
   
   #ifdef _WIN32
      face_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml");

   #else
      face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");
   
   #endif
   
   video_capture<process_frame, true> input;

   input.capture_sync();

   return 0;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
