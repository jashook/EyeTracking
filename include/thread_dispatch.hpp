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

#include <array>
#include <functional>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<typename __LockType> class thread_dispatch
{
   private:  // Constructor | Desctructor

      thread_dispatch(std::size_t thread_count) { _ctor(thread_count); }
      ~thread_dispatch() { _dtor(); }

   public:  // Static Member Functions

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
         static auto start_function = [this]()
         {
            // Function threads will start in

         }

         _m_threads = new std::thread*[thread_count];
         _m_queues = new ev10::eIIe::ring_buffer<std::function<void()>*, 1024>[thread_count];

         for (std::size_t count = 0; count < thread_count; ++count)
         {
            _m_threads[count] = new std::thread(start_function);

         }
   
         // Non-blocking, return before threads are created
      }

      void _join_all()
      {
         for (std::thread* thread : _m_threads)
         {
            thread->join();
         }
      }

      void _start_all(std::function<void()>& function)
      {
         // Take writer lock here.

         for (std::size_t index = 0; index < _m_thread_count; ++index)
         {
            _m_process_queues[index]->add(&function);
         }

      }

   private: // Member variables

      ev10::eIIe::ring_buffer<std::function<void()>*, 1024>* _m_process_queues;

      __LockType _m_start_lock;
      std::thread* _m_threads;

}; // end of class (find_eye_center)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __THREAD_DISPATCH_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file include/thread_dispatch.hpp
////////////////////////////////////////////////////////////////////////////////

