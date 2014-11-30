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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define ESC_KEY 27

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<bool(*__ProcessingFunction)(IplImage*), bool __Gui = false, size_t __Threads = 1> class video_capture
{
   private: // Member Variables

      std::size_t _m_index;
      std::vector<CvCapture*>* _m_captures;

   public: // Constructor | Destructor

      video_capture() : _m_index(-1), _m_captures(NULL)
      {
         _m_captures = new std::vector<CvCapture*>();
         
         bool error = true;
         
         for (std::size_t index = 0; index < 3; ++index)
         {
            // Set up to capture from the first webcam
            _m_captures->push_back(cvCaptureFromCAM(0));
            
            // If the capture is still NULL, then we are unable to read
            if (_m_captures->at(index))
            {
               error = false;
            }
         }
         
         if (error)
         {
            std::cerr << "Unable to capture on a connected device.  Please make sure everything is connected correctly and try again." << std::endl;
         }
         else
         {
            // Set resolution
            
            // Try 480p
            cvSetCaptureProperty(_m_captures->at(0), CV_CAP_PROP_FRAME_WIDTH, 640);
            cvSetCaptureProperty(_m_captures->at(0), CV_CAP_PROP_FRAME_HEIGHT, 480);
            
            // Try 720p
            cvSetCaptureProperty(_m_captures->at(1), CV_CAP_PROP_FRAME_WIDTH, 1280);
            cvSetCaptureProperty(_m_captures->at(1), CV_CAP_PROP_FRAME_HEIGHT, 720);
            
            // Try 1080p
            cvSetCaptureProperty(_m_captures->at(2), CV_CAP_PROP_FRAME_WIDTH, 1920);
            cvSetCaptureProperty(_m_captures->at(2), CV_CAP_PROP_FRAME_HEIGHT, 1080);
            
         }
      }

      ~video_capture()
      {
         // Clean up everything
         cvReleaseCapture(&_m_captures->at(0));
         cvReleaseCapture(&_m_captures->at(1));
         cvReleaseCapture(&_m_captures->at(2));
         cvDestroyAllWindows();
         
         delete _m_captures;
      }

   public: // Member Functions

      void capture_sync()
      {
         IplImage* frame = NULL;
      
         bool done = false;
      
         while (!done)
         {
            if (_m_index == -1)
            {
               // Choose the highest resolution
               
               _m_index = cvQueryFrame(_m_captures->at(2)) ? 2 : -1;
               
               if (_m_index != -1) continue;
               
               _m_index = cvQueryFrame(_m_captures->at(0)) ? 1 : -1;
               
               if (_m_index != -1) continue;
               
               _m_index = cvQueryFrame(_m_captures->at(0)) ? 0 : -1;
            }
            
            if (_m_index == -1) continue;
            
            // Get the frame
            frame = cvQueryFrame(_m_captures->at(_m_index));
         
            if (frame)
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

      bool _show_default_gui(IplImage* frame)
      {
         cvShowImage("Video", frame);
      
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