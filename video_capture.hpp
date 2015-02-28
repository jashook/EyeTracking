////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: video_capture.hpp
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

#ifndef __VIDEO_CAPTURE_HPP__
#define __VIDEO_CAPTURE_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdexcept>

#if _WIN32

// Windows

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\videoio.hpp>

#else

// Unix Based System

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define ESC_KEY 27

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<bool(*__ProcessingFunction)(cv::Mat&), bool __Gui = false, size_t __Threads = 1> class video_capture
{
   private: // Member Variables

      cv::VideoCapture _m_capture;

   public: // Constructor | Destructor

      video_capture()
      {
         const std::string error_message = "Unable to capture on a connected device.  Please make sure everything is connected correctly and try again.";

         _m_capture.open(-1);

         // Set resolution
         
         // Try 1080p
         bool width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
         bool height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

         if (width && height)
         {
            if (!_m_capture.open(0))
            {
               std::cerr << error_message << std::endl;
            }

            return;
         }

         std::cout << _m_capture.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;

         // Try 720p
         width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
         height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 768);

         if (width && height)
         {
            if (!_m_capture.open(0))
            {
               std::cerr << error_message << std::endl;
            }

            return;
         }

         width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
         height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

         if (width && height)
         {
            if (!_m_capture.open(-1))
            {
               std::cerr << error_message << std::endl;
            }

            return;
         }

         std::cerr << "Unable to set the frame width and height, defaulting to the standard resolution" << std::endl;
      }

      ~video_capture()
      {
         // Clean up everything

         _m_capture.release();
      }

   public: // Member Functions

      void capture_sync()
      {
         cv::Mat frame;
      
         bool done = false;
      
         while (!done)
         {
            // Get the frame
            _m_capture >> frame;
         
            if (!frame.empty())
            {
               // Start processing
               done = __ProcessingFunction(frame);
            
               if (__Gui)
               {
                  done = _show_default_gui(frame);
               
               }
            }
         }
      }
   
   private: // Private Member functions

      bool _show_default_gui(cv::Mat& frame)
      {
         cv::imshow("Video", frame);
      
         // Wait 10 ms for a key to be pressed
         int character = cvWaitKey(10);
      
         // Break if the esc key is pressed
         if (character == ESC_KEY) return true;
      
         return false;
      
      }

}; // end of class (video_capture)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __VIDEO_CAPTURE_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
