////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Author: Jarret Shook
//
// Module: ring_buffer.hpp
// 
// Timeperiod: 
// 
// 01-Mar-15: Version 1.0: Created 
// 01-Mar-15: Version 1.0: Last Updated 
// 
// Notes: 
//
// Easy implementation for a circular queue
// 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __RING_BUFFER_HPP__
#define __RING_BUFFER_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cstring>
#include <exception>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<typename __Type, size_t __Size> class ring_buffer
{
   public:  // Constructor | Destructor

      ring_buffer(size_t start_location = 0) { _ctor(start_location); }
      ~ring_buffer() { _dtor(); }

   public:  // Member Functions

      bool empty() { return _empty(); }
      __Type pop() { return _pop(); }
      void push(__Type value) { _push(value); }

   public:  // Public Operators

      operator bool() { return _empty(); }

   private: // Private Member Functions

      void _ctor(size_t start_location)
      {
         _m_start_index = 0;
         _m_end_index = start_location - 1 < __Size ? start_location - 1 : 0;
         _m_amount = 0;

         std::memset(_m_queue, 0, sizeof(__Type) * __Size);
      }

      void _dtor()
      {
         _m_start_index = 0;
         _m_end_index = 0;

         std::memset(_m_queue, 0, sizeof(__Type) * __Size);
      }

      bool _empty()
      {
         return _m_amount == 0;
      }

      __Type _pop()
      {
         if (_m_end_index == _m_start_index)
         {
            throw std::runtime_error("[ev10::eIIe::ring_buffer::_pop] Attempting to remove a value that does not exist");
         }

         __Type temp = _m_queue[_m_start_index];

         // Increase the index then mode by the size to
         // allow the queue to wrap to the beginning
         ++_m_start_index %= __Size;

         _m_amount = _m_amount == 0 ? _m_amount : _m_amount - 1;

         return temp;
      }

      void _push(__Type& value)
      {
         // Add in the next element
         _m_queue[_m_end_index] = value;

         // Increase the index then mod by the size to
         // allow the queue to wrap to the beginning if full
         ++_m_end_index %= __Size;

         // If more values were inserted then we have the capacity for
         // then drop the first value inserted
         _m_start_index = _m_end_index == _m_start_index ? _m_start_index + 1 : _m_start_index;

         _m_amount = _m_amount == __Size ? _m_amount : _m_amount + 1;
      }

   private: // Member Variables
   
      size_t _m_amount;

      size_t _m_end_index;
      __Type _m_queue[__Size];
      size_t _m_start_index;

}; // end of class ring_buffer

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe)
} // end of namespace(ev10)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __RING_BUFFER_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file ring_buffer.hpp
////////////////////////////////////////////////////////////////////////////////

