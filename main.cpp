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
#include "socket.hpp"
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
const int point_queue_length = 2;
const int delta_queue_length = 150;
ev10::eIIe::ring_buffer < cv::Point, point_queue_length > center_point_queue(point_queue_length);
ev10::eIIe::ring_buffer < double, delta_queue_length > center_delta_history(delta_queue_length);
ev10::eIIe::ring_buffer < double, delta_queue_length > variance_sum_terms(delta_queue_length);

//make a counter variable to delay the start of moving the on screen image
int screen_adjustment_hit_count = 0;
int running_delta_sum = 0;
int running_variance_sum = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//keeps track of previous and current center points (left eye only)
//calculates the nececary screen adjustment
//    ASSUMES:
//          Subject (eye) is 1 foot away from the camera
//          Subject (eye) has a radius of 1 inch
//          Camera is in 640x480 (resolution of roughly 60 pixels/inch at 1 foot)
//          Monitor resolution = 1920x1080, 24 inch monitor
inline void calculate_screen_adjustment(cv::Point current)
{

   /*TODO*/
      //establish a history of points
         //dont start until a few seconds in (several hundred data)
         //calculate mean/stdev
         //check new point to see if it is reasonable, or if its an error (within 3 stdevs of mean)
         //continue, or throw away point
   /*END_TODO*/

   //deal with first case (add to queue and return)
   if (screen_adjustment_hit_count == 0)
   {
      ++screen_adjustment_hit_count;
      center_point_queue.push(current);
      return;
   }

   // Get previous position
   cv::Point previous = center_point_queue.pop();
   int previous_x = previous.x;
   //get current position
   int current_x = current.x;
   //calculate Difference
   double delta_x = abs(current_x - previous_x);

   //////build up a history of points (store data for 5 seconds)
   //if (screen_adjustment_hit_count ++ < delta_queue_length + 1)
   //{
   //   //store delta value
   //   center_delta_history.push(delta_x);
   //   //add to running delta sum
   //   running_delta_sum += delta_x;

   //   //calculate average
   //   double mean_delta = running_delta_sum / screen_adjustment_hit_count;

   //   //calculate variance sum term
   //   double variance_term = pow(delta_x - mean_delta, 2);

   //   //store variance sum term
   //   variance_sum_terms.push(variance_term);

   //   //add to running variance sum
   //   running_variance_sum += variance_term;

   //   //store current center point
   //   center_point_queue.push(current);

   //   //continue to next data point
   //   return;
   //}
   ////ELSE - the program is ready to run - go for it.
   //
   ////calculate the variance -> stdev
   //double stdev = sqrt(running_variance_sum / delta_queue_length);
   //double mean_delta = running_delta_sum / delta_queue_length;

   ////if delta is outside of 3 stdevs from the mean, return
   //if (abs(mean_delta - delta_x) > 3 * stdev)
   //{
   //   //restore previous point (ignore current point?)
   //   center_point_queue.push(previous);

   //   //
   //   return;
   //}

   ////subtract from running sums
   //running_variance_sum -= variance_sum_terms.pop();
   //running_delta_sum -= center_delta_history.pop();

   ////add to running delta sum
   //running_delta_sum += delta_x;
   ////calcluate mean delta
   //double mean_delta = running_delta_sum / delta_queue_length;

   ////calculate variance sum term
   //double variance_term = pow(delta_x - mean_delta, 2);

   ////add to running variance sum
   //running_variance_sum += variance_term;

   ////store points
   //variance_sum_terms.push(variance_term);
   //center_delta_history.push(delta_x);

   //adjust screen (everything below here is communication with the 'front end')

   //connect to the socket
   static ev9::socket* socket = new ev9::socket(7000);
   static bool initialized = false;

   if (!initialized)
   {
      try
      {
         socket->connect();

         initialized = true;
      }

      catch (std::exception& e)
      {
         std::cout << "Unable to connect with the front end.  Failing fast." << std::endl;

         exit(1);
      }
   }
   //if the delta in pixels = 0, return
   if (delta_x == 0)
   {
      center_point_queue.push(previous);
      return;
   }
   //calculate adjustment on camera plane
   double delta_c_in_inches = delta_x / spatialResolution;

   //calculate screen adjustment in inches
   double delta_s_in_inches = delta_c_in_inches * cameraDistance;

   //calcluate screen adjustment in pixels
   double delta_s_in_pixels = delta_s_in_inches * monitorInchesToPixels;

   //calculate adjustment in open GL ratio
   double adjustment_in_openGL_ratio = delta_s_in_pixels / monitorResolution;

   if (current_x < previous_x)
   {
      // Move Right
      //if right, send value as positive
      static std::string adjustmentString = std::to_string(adjustment_in_openGL_ratio);
      socket->write(adjustmentString);

      std::cout << "right\t" << std::flush;
   }

   else
   {
      // Move left
      //if left, move value as negative
      adjustment_in_openGL_ratio *= -1.0;
      static std::string adjustmentString = std::to_string(adjustment_in_openGL_ratio);
      socket->write(adjustmentString);

      std::cout << "left\t" << std::flush;
   }

   //store current point
   center_point_queue.push(current);
}

inline cv::Rect* get_face_area(cv::Mat& current_image, int min_object_size)
{
   static cv::Rect* s_saved_rect = nullptr;
   static size_t s_frames = 0;

   ++s_frames;

   if (s_frames > 10)
   {
      s_frames = 1;
   }

   if (s_frames == 10 || s_frames == 1)
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
   // Dynamically scale min object size by the width of the image (hueristically determined to be img_width / 4)
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

   //change eye coordinate to image
   leftPupil.x += face->x;
   leftPupil.y += face->y;
   //update centerpoint
   calculate_screen_adjustment(leftPupil);
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
