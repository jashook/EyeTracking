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

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect/objdetect.hpp>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define ESC_KEY 27

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<bool(*__ProcessingFunction)(IplImage*), bool __Gui = false, size_t __Threads = 1> class video_capture
{
   private: // Member Variables

      CvCapture* _m_capture = NULL;

   public: // Constructor | Destructor

      video_capture() : _m_capture(NULL) 
      { 
         // Set up to capture from the first webcam
         _m_capture = cvCaptureFromCAM(0);

         // If the capture is still NULL, then we are unable to read
         if (!_m_capture)
         {
             std::cerr << "Unable to capture on a connected device.  Please make sure everything is connected correctly and try again." << std::endl;
         }
      }

      ~video_capture()
      {
         // Clean up everything
         cvReleaseCapture(&_m_capture);
         cvDestroyAllWindows();
      }

   public: // Member Functions

      void capture_sync()
      {
         IplImage* frame = NULL;

         bool done = false;

         while (!done)
         {
            // Get the frame
            frame = cvQueryFrame(_m_capture);

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

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __VIDEO_CAPTURE_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////