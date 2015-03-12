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
// This files provides an implementation of a read/writer lock.  The lock favors
// Writers.
//
// This class was written before C++14 had compiler support, therefore the
// shared_mutex class was not yet available.  Future support would allow for
// use of the standard function.
//
// All readers are forced to wait if there is one writer. Therefore, writers are
// favored.
//
// Use case: In situations where there are multiple readers and few writers,
// where writers write infrequently and readers read very frequently.
//
// All readers have no access to mutable data to conform to the definition of 
// a read/write lock.
//
// This code is for us in Windows (win64, winArm, win32) and POSIX (x86, x64, ARM)
// 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __READ_WRITE_LOCK_HPP__
#define __READ_WRITE_LOCK_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <condition_variable>

#include "cas_atomic.hpp"
#include "cas_lock.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class rw_lock
{
   public:  // Type definitions and Constants

      enum accessor_type { READER, WRITER }

   public:  // Constructor | Destructor

      rw_lock_w_favored() { _ctor(); }
      ~rw_lock_w_favored() { _dtor(); }

   public:  // Public Member Functions

      template<accessor_type __AccessorType> void lock()
      {
         if (__AccessorType == READER)
         {
            // Check if there are any writers
            
            if (_m_writer_amount)
            {
               // Writer may be waiting, busy wait for the writer lock
   
               _m_writer_lock.lock();

               while (_m_writer_amount && !_m_reader_amount)
               {
                  _m_writer_lock.unlock();
            
                  // Ahead of the writer lock. Take the lock after waiting
                  std::this_thread::sleep_for(std::chrono::microseconds(20));

                  _m_writer_lock.lock();
               }

               // Increase the Reader amount to avoid a deadlock
               ++_m_reader_amount;

               // Writer is done at this point.
               // Unlock for other readers

               _m_writer_lock.unlock();

               // Have the reader critical section, return with the lock.
               
               return;
            }

            ++_m_reader_amount;

            // Multiple readers at this point
            // Reader Critical Section
            // Return with reader critical section finished
            
         }

         else
         {
            // Getting a Writer lock.


         }
      }

      template<accessor_type __AccessorType> void unlock()
      {

      }

   private: // Member Variables

      ev10::eIIe::cas_lock _m_writer_lock;
 
      ev10::eIIe::cas_atomic<size_t> _m_reader_amount;
      ev10::eIIe::cas_atomic<size_t> _m_writer_amount;    

}; // end of class(rw_lock)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe)
} // end of namespace(ev10)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __READ_WRITE_LOCK_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file include/read_write_lock.hpp
////////////////////////////////////////////////////////////////////////////////

