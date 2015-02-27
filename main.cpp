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
   
   
  
   cv::Mat mat_image(image);
   cv::Mat mat_gray = mat_image;

   // Convert to gray scale
   cvtColor(mat_image, mat_gray, CV_BGR2GRAY);
   
   #ifdef DEBUG_FLAG
   
      cv::imshow("debug_gray", mat_gray);
      cv::imshow("debug_color", mat_image);
   
   #endif

   // Detect faces
   std::vector<cv::Rect> objects;

   face_cascade.detectMultiScale(mat_gray, objects, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(300, 300));

   // Check to see that faces were found - if not, return
   if (objects.size() < 1)
   {
      return;
   }
      
   // Find eye regions (numbers included from constants.h)
   cv::Rect face = objects[0];
   
   cv::Mat faceROI_gray = mat_gray(face);
   cv::Mat faceROI = mat_image(face);
   
   int eye_region_width = face.width * (kEyePercentWidth / 100.0);
   int eye_region_height = face.width * (kEyePercentHeight / 100.0);
   int eye_region_top = face.height * (kEyePercentTop / 100.0);
   
   cv::Rect leftEyeRegion(face.width * (kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);
   cv::Rect rightEyeRegion(face.width - eye_region_width - face.width * (kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);

   //// Make a matrix out of the left eye
   //cv::Mat lEyeROI = faceROI_gray(leftEyeRegion);
   //cv::Mat lEyeROI_color = faceROI(leftEyeRegion);
   //
   //#ifdef DEBUG_FLAG
   //
   //   cv::imshow("debug_color", lEyeROI_color);

   //#endif
   //
   //cv::medianBlur(lEyeROI, lEyeROI, 9);
   //
   ////display result
   //cv::imshow("debug_blur", lEyeROI);

   ////apply hough transform
   //std::vector<cv::Vec3f> circles;
   //cv::HoughCircles(lEyeROI, circles, CV_HOUGH_GRADIENT,
   //   2,       //accumulator resolution
   //   150,     //minimum distance between two circles
   //   125,     //canny high threshhold
   //   75,      //minimum number of votes
   //   0, 0); //min and max radius

   ////draw center of detected circles
   //for (size_t i = 0; i < circles.size(); i++)
   //{
   //   //draw points
   //   cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
   //   cv::circle(lEyeROI_color, center, 3, cv::Scalar(0, 255, 0));
   //   cv::circle(lEyeROI, center, 3, cv::Scalar(0, 0, 255));
   //   cv::imshow("debug_blur", lEyeROI);
   //}

   //~~~~~~~~~EYE LIKE~~~~~~~~~~~

   //-- Find Eye Centers
   cv::Point leftPupil = findEyeCenter(faceROI_gray, leftEyeRegion, "Left Eye");
   cv::Point rightPupil = findEyeCenter(faceROI_gray, rightEyeRegion, "Right Eye");

   // change eye centers to face coordinates
   rightPupil.x += rightEyeRegion.x;
   rightPupil.y += rightEyeRegion.y;
   leftPupil.x += leftEyeRegion.x;
   leftPupil.y += leftEyeRegion.y;

   // draw eye centers
   circle(faceROI, rightPupil, 3, cv::Scalar(0, 255, 0));
   circle(faceROI, leftPupil, 3, cv::Scalar(0, 255, 0));

   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~


   //-- Draw eye regions (left: green, right: red)
   cv::rectangle(faceROI, leftEyeRegion, cv::Scalar(0, 255, 0));
   cv::rectangle(faceROI, rightEyeRegion, cv::Scalar(0, 0, 255));
   
   // Print all the objects detected
   for (int i = 0; i < objects.size(); i++)
   {
      cv::rectangle(mat_image, objects[i], cv::Scalar( 255, 0, 0 ));
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

inline void hough_test()
{
   // Load image (test eye image located in resources)
   cv::Mat src, src_gray;
   
   // Read image
   src = cv::imread("eye.jpg", CV_LOAD_IMAGE_COLOR);
   
   // Display image
   cv::namedWindow("color", cv::WINDOW_AUTOSIZE);
   imshow("color", src);

   // Proceess image
   // Convert to Grayscale
   cvtColor(src, src_gray, CV_BGR2GRAY);
   
   // Display the Grayscale Image
#ifdef DEBUG_FLAG
   cv::namedWindow("gray");
   imshow("gray", src_gray);
#endif
   
   // Blur (median filter for speed)
   medianBlur(src_gray, src_gray, 21);
   
   // Display blur
#ifdef DEBUG_FLAG
   cv::namedWindow("gray_blur");
   imshow("gray_blur", src_gray);
#endif
   
   // Hough transform
   std::vector<cv::Vec3f> circles;
   HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 2, src_gray.rows / 8, 100, 100, 0, 500);
   
   // Draw circles
   for (size_t i = 0; i < circles.size(); i++)
   {
      cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // Draw center
      
      circle(src, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
      
      // Draw seg circle
      circle(src, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);
   }
   
   imshow("color", src);

   // Pause before return
   cv::waitKey(0);
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

   //hough_test();
   return 0;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
