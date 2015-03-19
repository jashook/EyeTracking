////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Author: Jarret Shook
// 
// Timeperiod: 
// 
// 05-Mar-15: Version 1.0: Created 
// 05-Mar-15: Version 1.0: Last Updated 
// 
// Notes: 
//
// Singleton Type
//
// std::function<void()> is the c++11 way to pass a lambda.
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __THREAD_DISPATCH_HPP__
#define __THREAD_DISPATCH_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "rw_lock.hpp"

#include <array>
#include <functional>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class thread_dispatch
{
   private:  // Constructor | Desctructor

      thread_dispatch(std::size_t thread_count) { _ctor(thread_count); }
      ~thread_dispatch() { _dtor(); }

   public:  // Static Member Functions

      static void clean_up()
      {
         s_dispatch->join_all();

         delete s_displatch;

      }

      static thread_dispatch* const get_dispatch() 
      { 
         static thread_dispatch* s_dispatch = nullptr;
         
         if (thread_dispatch == nullptr)
         {
            std::size_t thread_count = std::thread::hardware_concurrancy();

            s_dispatch = new thread_dispatch(thread_count);
         }
         
         return s_dispatch;
      }

   public:  // Member Functions

      void join_all() { _join_all(); }
      void start_all() { _start_all(); }

   private: // Private Member functions

      void _ctor(size_t thread_count)
      {
         // Function threads will start in
         
         static auto start_function = [this](int thread_index)
         {
            while (1)
            {
               // Take a reader lock
               _m_lock.lock<ev10::eIIe::READER>();

               // Check if there is anything to work on

               if (_m_finished)
               {
                  _m_lock.unlock<ev10::eIIe::READER>();
   
                  break;
               }

               else if (!_m_queues[thread_index].empty())
               {
                  // Pop off the function to work on.

                  std::function<void>* function = _m_queues[thread_index].pop();

                  _m_lock.unlock<ev10::eIIe::READER>();

                  (*function)();

               }

               else
               {
                  _m_lock.unlock<ev10::eIIe::READER>();
               }

            }
         }

         _m_finished = false;

         _m_threads = new std::thread*[thread_count];
         _m_queues = new ev10::eIIe::ring_buffer<std::function<void()>*, 1024>[thread_count];

         for (std::size_t count = 0; count < thread_count; ++count)
         {
            _m_threads[count] = new std::thread(start_function, count);

         }
   
         // Non-blocking, return before threads are created
      }

      void _dtor()
      {
         delete [] _m_queues;

         for (std::size_t count = 0; count < _m_thread_count; ++count)
         {
            delete [] _m_threads[count];
         }

         delete [] _m_threads;
      }

      void _join_all()
      {
         _m_lock.lock<ev10::eIIe::WRITER>();

         _m_finished = true;

         _m_lock.lock<ev10::eIIe::WRITER>(); 

         for (std::thread* thread : _m_threads)
         {
            thread->join();
         }
      }

      void _start_all(std::function<void()>& function)
      {
         // Take writer lock here.

         _m_lock.lock<ev10::eIIe::WRITER>();

         for (std::size_t index = 0; index < _m_thread_count; ++index)
         {
            _m_process_queues[index]->add(&function);
         }

         _m_lock.unlock<ev10::eIIe::WRITER>();

      }

   private: // Member variables

      bool _m_finished;

      ev10::eIIe::ring_buffer<std::function<void()>*, 1024>* _m_process_queues;

      ev10::eIIe::rw_lock _m_lock;
      std::thread* _m_threads;

}; // end of class (find_eye_center)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __THREAD_DISPATCH_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file include/thread_dispatch.hpp
////////////////////////////////////////////////////////////////////////////////

