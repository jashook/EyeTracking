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
#include <chrono>
#include <functional>
#include <thread>

#include "ring_buffer.hpp"
#include "rw_lock.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<typename __Type, typename ... __Arguments> class thread_dispatch
{
   private:  // Constructor | Desctructor

      thread_dispatch(std::size_t thread_count) { _ctor(thread_count); }
      ~thread_dispatch() { _dtor(); }

   public:  // Static Member Functions

      static void clean_up()
      {
         thread_dispatch* dispatch = get_dispatch();

         dispatch->_join_all_hard();

         delete dispatch;

         get_dispatch(true);
      }

      static thread_dispatch* get_dispatch(bool reset = false)
      {
         static thread_dispatch* s_dispatch = nullptr;

         if (reset)
         {
            s_dispatch = nullptr;

            return nullptr;
         }

         if (s_dispatch == nullptr)
         {
            std::size_t thread_count = std::thread::hardware_concurrency();

            s_dispatch = new thread_dispatch(thread_count);
         }

         return s_dispatch;
      }

      static void set_value(__Type value, __Arguments ... arguments)
      {
         thread_dispatch* dispatch = get_dispatch();

         dispatch->_m_values = std::make_tuple(value, arguments ...);
      }

      static size_t thread_count()
      {
         thread_dispatch* dispatch = get_dispatch();

         return dispatch->get_thread_count();
      }

   public:  // Member Functions

      void add_process_all(std::function<void(std::tuple<__Type, __Arguments ...>, std::size_t)> function) { _start_all(function); }
      std::size_t get_thread_count() { return _get_thread_count(); }
      void join_all() { _join_all(); }

   private: // Private Member functions

      void _ctor(size_t thread_count)
      {
         // Function threads will start in

         auto start_function = [this](std::size_t thread_index)
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

               else if (!_m_process_queues[thread_index].empty())
               {
                  // Pop off the function to work on.

                  auto function = _m_process_queues[thread_index].pop();

                  _m_lock.unlock<ev10::eIIe::READER>();

                  function(_m_values, thread_index);

                  _m_lock.lock<ev10::eIIe::WRITER>();

                  --_m_join_amount;

                  _m_lock.unlock<ev10::eIIe::WRITER>();
               }

               else
               {
                  _m_lock.unlock<ev10::eIIe::READER>();
               }

            }
         };

         _m_finished = false;

         _m_thread_count = thread_count;
         _m_threads = new std::thread*[thread_count];
         _m_process_queues = new ev10::eIIe::ring_buffer<std::function<void(std::tuple<__Type, __Arguments ...>, std::size_t)>, 1024>[thread_count];

         _m_join_amount = 0;

         for (std::size_t count = 0; count < thread_count; ++count)
         {
            _m_threads[count] = new std::thread(start_function, count);
         }

         // Non-blocking, return before threads are created
      }

      void _dtor()
      {
         delete [] _m_process_queues;

         for (std::size_t count = 0; count < _m_thread_count; ++count)
         {
            delete _m_threads[count];
         }

         delete [] _m_threads;
      }

      void _join_all()
      {
         _m_lock.lock<ev10::eIIe::READER>();

         while (_m_join_amount)
         {
            _m_lock.unlock<ev10::eIIe::READER>();

            std::this_thread::sleep_for(std::chrono::microseconds(10));

            _m_lock.lock<ev10::eIIe::READER>();
         }

         _m_lock.unlock<ev10::eIIe::READER>();
      }

      std::size_t _get_thread_count()
      {
         return _m_thread_count;
      }

      void _join_all_hard()
      {
         _m_lock.lock<ev10::eIIe::WRITER>();

         _m_finished = true;

         _m_lock.unlock<ev10::eIIe::WRITER>();

         for (std::size_t index = 0; index < _m_thread_count; ++index)
         {
            _m_threads[index]->join();
         }
      }

      void _start_all(std::function<void(std::tuple<__Type, __Arguments ...>, std::size_t)> function)
      {
         // Take writer lock here.

         _m_lock.lock<ev10::eIIe::WRITER>();

         _m_join_amount = 4;

         for (std::size_t index = 0; index < _m_thread_count; ++index)
         {
            _m_process_queues[index].push(function);
         }

         _m_lock.unlock<ev10::eIIe::WRITER>();

      }

   private: // Member variables

      bool _m_finished;

      ev10::eIIe::ring_buffer<std::function<void(std::tuple<__Type, __Arguments ...>, std::size_t)>, 1024>* _m_process_queues;

      ev10::eIIe::rw_lock _m_lock;
      std::thread** _m_threads;
      std::size_t _m_thread_count;
      std::size_t _m_join_amount;

      std::tuple<__Type, __Arguments ...> _m_values;

}; // end of class (find_eye_center)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe) 
} // end of namespace(ev10)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __THREAD_DISPATCH_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file include/thread_dispatch.hpp
////////////////////////////////////////////////////////////////////////////////

