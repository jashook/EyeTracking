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
#include "timing_helper.hpp"
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

//store previous centerPoint(s)
const int queue_length = 2;
ev10::eIIe::ring_buffer < cv::Point, queue_length > center_point_history(queue_length);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//keeps track of previous and current center points (left eye only)
//calculates the nececary screen adjustment
//    ASSUMES:
//          Subject (eye) is 1 foot away from the camera
//          Subject (eye) has a radius of 1 inch
//          Camera is in 640x480 (resolution of roughly 60 pixels/inch at 1 foot)
//          Monitor resolution = 1920x1080, 24 inch monitor
inline void calculateScreenAdjustment(cv::Point current)
{
   //get previous position
   cv::Point previous = center_point_history.pop();
   int previous_x = previous.x;
   //get current position
   int current_x = current.x;

   //difference
   float delta_x = abs(current_x - previous_x);

   //calculate screen adjustment
   float adjustment_in_inches = delta_x * adjustmentPixelsToInches;
   static int adjustment_in_pixels = round(adjustment_in_inches * adjustment_in_pixels);

   if (current_x > previous_x)
   {
      //move right?
   }
   else
   {
      //move left?
   }

   //store current point
   center_point_history.push(current);
}

inline cv::Rect* get_face_area(cv::Mat& current_image, int min_object_size)
{
   static cv::Rect* s_saved_rect = nullptr;
   static size_t s_frames = 0;

   ++s_frames;

   if (s_frames > 50)
   {
      s_frames = 1;
   }

   if (s_frames == 50 || s_frames == 1)
   {
      // Detect faces
      std::vector<cv::Rect> faces;

	   face_cascade.detectMultiScale(current_image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(min_object_size, min_object_size));

      // Check to see that faces were found - if not, return
      if (faces.size() < 1)
      {
         return nullptr;
      }
      
      // Find eye regions (numbers included from constants.h) (only use the first face)
      if (!s_saved_rect)
      {
         s_saved_rect = new cv::Rect(faces[0]);
      }

      else
      {
      *s_saved_rect = faces[0];
      }

      s_frames = 1;
   
   }

   return s_saved_rect;
}

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
   // Dynamically scale min object size by the width of the image (hueristically determined to be img_width / 4)  ((hueristically is a cool word for made up))
	int min_object_size = image.cols / 4;

   cv::Rect* face = get_face_area(mat_gray, min_object_size);

   if (!face) return;

   cv::Mat face_roi_gray = mat_gray(*face);

   cv::Mat face_roi = image(*face);
   
   int eye_region_width = face->width * (kEyePercentWidth / 100.0);
   int eye_region_height = face->width * (kEyePercentHeight / 100.0);
   int eye_region_top = face->height * (kEyePercentTop / 100.0);
   
   cv::Rect left_eye_region(face->width * (kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);

   //cv::Rect right_eye_region(face->width - eye_region_width - face->width * (kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);

   /////////////////////////////////////////////////////////////////////////////
   // EYE LIKE
   /////////////////////////////////////////////////////////////////////////////

   //-- Find Eye Centers
   cv::Point leftPupil = findEyeCenter(face_roi_gray, left_eye_region, "Left Eye");
   //cv::Point rightPupil = findEyeCenter(face_roi_gray, right_eye_region, "Right Eye");
   //cv::Point rightPupil(0, 0);

   // change eye centers to face coordinates
   //rightPupil.x += right_eye_region.x;
   //rightPupil.y += right_eye_region.y;
   leftPupil.x += left_eye_region.x;
   leftPupil.y += left_eye_region.y;

   // draw eye centers
   //circle(face_roi, rightPupil, 3, cv::Scalar(0, 255, 0));
   circle(face_roi, leftPupil, 3, cv::Scalar(0, 255, 0));

   /////////////////////////////////////////////////////////////////////////////
   // Draw eye regions (left: green, right: red)
   /////////////////////////////////////////////////////////////////////////////
   
   cv::rectangle(face_roi, left_eye_region, cv::Scalar(0, 255, 0));
   //cv::rectangle(face_roi, right_eye_region, cv::Scalar(0, 0, 255));
   
   // Print all the objects detected
   cv::rectangle(image, *face, cv::Scalar(255, 0, 0));

   //update centerpoint
   //calculateScreenAdjustment(leftPupil);
}

inline bool process_frame(cv::Mat& frame)
{
	//face detection -> find eye regions -> find eye center
   //TODO - add the same kind of averaging fucntionality that exists for the fps timer (ring buffer and average of previous frames...)
   double time = ev10::eIIe::timing_helper<ev10::eIIe::SECOND>::time(face_detection, frame);

#ifdef FPS_TIMING
	std::cout << "allMath/sec: " << 1 / time << "\t" << std::flush;
#else
	std::cout << "allMath/sec: " << 1 / time << "\r" << std::flush;
#endif
   
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
