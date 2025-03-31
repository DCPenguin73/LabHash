/***********************************************************************
 * Header:
 *    VECTOR
 * Summary:
 *    Our custom implementation of std::vector
 *      __      __     _______        __
 *     /  |    /  |   |  _____|   _  / /
 *     `| |    `| |   | |____    (_)/ /
 *      | |     | |   '_.____''.   / / _
 *     _| |_   _| |_  | \____) |  / / (_)
 *    |_____| |_____|  \______.' /_/
 *
 *    This will contain the class definition of:
 *        vector                 : A class that represents a Vector
 *        vector::iterator       : An iterator through Vector
 * Author
 *    Daniel Carr, Jarom Anderson, Arlo Jolley
 ************************************************************************/

#pragma once

#include <cassert>  // because I am paranoid
#include <cstddef>
#include <new>      // std::bad_alloc
#include <memory>   // for std::allocator
#include <initializer_list> // for std::initializer

class TestVector; // forward declaration for unit tests
class TestStack;
class TestPQueue;
class TestHash;

namespace custom
{

   /*****************************************
    * VECTOR
    * Just like the std :: vector <T> class
    ****************************************/
   template <typename T, typename A = std::allocator<T>>
   class vector
   {
      friend class ::TestVector; // give unit tests access to the privates
      friend class ::TestStack;
      friend class ::TestPQueue;
      friend class ::TestHash;
   public:

      //
      // Construct
      //
      vector(const A& a = A());                                      // bob
      vector(size_t numElements, const A& a = A());  // billy
      vector(size_t numElements, const T& t, const A& a = A());  // joe
      vector(const std::initializer_list<T>& l, const A& a = A());  // moe
      vector(const vector& rhs);                                   // jr
      vector(vector&& rhs);                                   // guss
      ~vector();

      //
      // Assign
      //
      void swap(vector& rhs);
      vector& operator = (const vector& rhs);
      vector& operator = (vector&& rhs);

      //
      // Iterator
      //
      class iterator;
      iterator begin()
      {
         return iterator(data);
      }
      iterator end()
      {
         return iterator(data + numElements);
      }

      //
      // Access
      //
      T& operator [] (size_t index);
      const T& operator [] (size_t index) const;
      T& front();
      const T& front() const;
      T& back();
      const T& back() const;

      //
      // Insert
      //
      void push_back(const T& t);
      void push_back(T&& t);
      void reserve(size_t newCapacity);
      void resize(size_t newElements);
      void resize(size_t newElements, const T& t);

      //
      // Remove
      //
      void clear()
      {
         if (data != nullptr)
         {
            for (size_t i = 0; i < numElements; i++)
            {
               alloc.destroy(&data[i]);
            }
         }
         numElements = 0;
      }
      void pop_back()
      {
         if (data != nullptr)
         {
            alloc.destroy(&data[numElements - 1]);
            numElements--;
         }
      }
      void shrink_to_fit();

      //
      // Status
      //
      size_t  size()          const { return numElements; }
      size_t  capacity()      const { return numCapacity; }
      bool empty()            const { return (numElements == 0); }

   private:

      A    alloc;                // use allocator for memory allocation
      T* data;                 // user data, a dynamically-allocated array
      size_t  numCapacity;       // the capacity of the array
      size_t  numElements;       // the number of items currently used
   };

   /**************************************************
    * VECTOR ITERATOR
    * An iterator through vector.  You only need to
    * support the following:
    *   1. Constructors (default and copy)
    *   2. Not equals operator
    *   3. Increment (prefix and postfix)
    *   4. Dereference
    * This particular iterator is a bi-directional meaning
    * that ++ and -- both work.  Not all iterators are that way.
    *************************************************/
   template <typename T, typename A>
   class vector <T, A> ::iterator
   {
      friend class ::TestVector; // give unit tests access to the privates
      friend class ::TestStack;
      friend class ::TestPQueue;
      friend class ::TestHash;
   public:
      // constructors, destructors, and assignment operator
      iterator() : p(nullptr) {  }
      iterator(T* p) : p(p) {  }
      iterator(const iterator& rhs) : p(rhs.p) {  }
      iterator(size_t index, vector<T>& v) : p(&v.data[index]) {  }
      iterator& operator = (const iterator& rhs)
      {
         if (this != &rhs)
            p = rhs.p;
         return *this;
      }

      // equals, not equals operator
      bool operator != (const iterator& rhs) const { return p != rhs.p; }
      bool operator == (const iterator& rhs) const { return p == rhs.p; }

      // dereference operator
      T& operator * () { return *p; }
      const T& operator*() const { return *p; }

      // prefix increment
      iterator& operator ++ ()
      {
         ++p;
         return *this;
      }

      // postfix increment
      iterator operator ++ (int postfix)
      {
         iterator temp = *this;
         ++p;
         return temp;
      }

      // prefix decrement
      iterator& operator -- ()
      {
         --p;
         return *this;
      }

      // postfix decrement
      iterator operator -- (int postfix)
      {
         iterator temp = *this;
         --p;
         return temp;
      }

   private:
      T* p;
   };


   /*****************************************
    * VECTOR :: NON-DEFAULT constructors
    * non-default constructor: set the number of elements,
    * construct each element, and copy the values over
    ****************************************/
   template <typename T, typename A>                                             // bob
   vector <T, A> ::vector(const A& a)
   {
      data = nullptr;
      numElements = 0;
      numCapacity = 0;
   }


   /*****************************************
    * VECTOR :: NON-DEFAULT constructors
    * non-default constructor: set the number of elements,
    * construct each element, and copy the values over
    ****************************************/
   template <typename T, typename A>                                             // joe
   vector <T, A> ::vector(size_t num, const T& t, const A& a)
   {
      if (num > 0)
      {
         data = alloc.allocate(num);
         for (size_t i = 0; i < num; i++)
            alloc.construct(&data[i], t);
      }
      else
         data = nullptr;
      //data = new T[100];
      numElements = num;
      numCapacity = num;
   }

   /*****************************************
    * VECTOR :: INITIALIZATION LIST constructors
    * Create a vector with an initialization list.
    ****************************************/
   template <typename T, typename A>
   vector <T, A> ::vector(const std::initializer_list<T>& l, const A& a)      // moe
      : numElements(l.size()), numCapacity(l.size())
   {
      if (numElements > 0)
      {
         data = alloc.allocate(numElements);
         for (auto it = l.begin(); it != l.end(); ++it)
            alloc.construct(&data[it - l.begin()], *it);
      }
   }

   /*****************************************
    * VECTOR :: NON-DEFAULT constructors
    * non-default constructor: set the number of elements,
    * construct each element, and copy the values over
    ****************************************/
   template <typename T, typename A>                                             // billy
   vector <T, A> ::vector(size_t num, const A& a)
   {
      if (num > 0)
      {
         //insert list maker of all 0's
         data = alloc.allocate(num);
         for (size_t i = 0; i < num; i++)
            alloc.construct(&data[i]);
      }
      else
      {
         data = nullptr;
      }
      //data = nullptr;
      numElements = num;
      numCapacity = num;
   }

   /*****************************************
    * VECTOR :: COPY CONSTRUCTOR
    * Allocate the space for numElements and
    * call the copy constructor on each element
    ****************************************/
   template <typename T, typename A>
   vector <T, A> ::vector(const vector& rhs)                                  // jr
   {
      if (rhs.numCapacity > 0)
      {
         data = alloc.allocate(rhs.numElements);
         numElements = rhs.numElements;
         numCapacity = rhs.numElements;
         for (size_t i = 0; i < rhs.numElements; i++)
            alloc.construct(&data[i], rhs.data[i]);
      }
      else
      {
         data = nullptr;
         numElements = 0;
         numCapacity = 0;
      }

   }

   /*****************************************
    * VECTOR :: MOVE CONSTRUCTOR
    * Steal the values from the RHS and set it to zero.
    ****************************************/
   template <typename T, typename A>
   vector <T, A> ::vector(vector&& rhs)                                      // guss
   {
      data = rhs.data;
      rhs.data = nullptr;
      numElements = rhs.numElements;
      rhs.numElements = 0;
      numCapacity = rhs.numCapacity;
      rhs.numCapacity = 0;
   }

   /*****************************************
    * VECTOR :: DESTRUCTOR
    * Call the destructor for each element from 0..numElements
    * and then free the memory
    ****************************************/
   template <typename T, typename A>
   vector <T, A> :: ~vector()
   {
      if (data != nullptr)
      {
         for (size_t i = 0; i < numElements; i++)
            alloc.destroy(&data[i]);
         alloc.deallocate(data, numCapacity);
      }
   }

   /***************************************
    * VECTOR :: RESIZE
    * This method will adjust the size to newElements.
    * This will either grow or shrink newElements.
    *     INPUT  : newCapacity the size of the new buffer
    *     OUTPUT :
    **************************************/
   template <typename T, typename A>
   void vector <T, A> ::resize(size_t newElements)
   {
      // If capacity is the same, do nothing.
      if (newElements == numElements) {
         return;
      }

      if (newElements < numElements) {
         // Shrink: destroy excess elements
         for (size_t i = newElements; i < numElements; i++) {
            alloc.destroy(data + i);
         }
         numElements = newElements;
         return;
      }

      // Grow
      if (newElements > numCapacity) {
         reserve(newElements);
      }

      // Construct new elements
      for (size_t i = numElements; i < newElements; i++) {
         alloc.construct(data + i);
      }
      numElements = newElements;
   }

   template <typename T, typename A>
   void vector <T, A> ::resize(size_t newElements, const T& t)
   {
      // If capacity is the same, do nothing.
      if (newElements == numElements) {
         return;
      }

      if (newElements < numElements) {
         // Shrink: destroy excess elements
         for (size_t i = newElements; i < numElements; i++) {
            alloc.destroy(data + i);
         }
         numElements = newElements;
         return;
      }

      // Grow
      if (newElements > numCapacity) {
         reserve(newElements);
      }

      // Construct new elements (copy of t)
      for (size_t i = numElements; i < newElements; i++) {
         alloc.construct(data + i, t);
      }
      numElements = newElements;
   }

   /***************************************
    * VECTOR :: RESERVE
    * This method will grow the current buffer
    * to newCapacity.  It will also copy all
    * the data from the old buffer into the new
    *     INPUT  : newCapacity the size of the new buffer
    *     OUTPUT :
    **************************************/
   template <typename T, typename A>
   void vector <T, A> ::reserve(size_t newCapacity)
   {
      if (newCapacity <= numCapacity) {
         return;
      }
      // allocate new array
      T* dataNew = alloc.allocate(newCapacity);

      // move old elements to new array
      for (size_t i = 0; i < numElements; i++) {
         new ((void*)(dataNew + i)) T(std::move(data[i]));
      }

      for (size_t i = 0; i < numElements; i++) {
         alloc.destroy(&data[i]);
      }
      alloc.deallocate(data, numCapacity);

      data = dataNew;
      numCapacity = newCapacity;

   }

   /***************************************
    * VECTOR :: SHRINK TO FIT
    * Get rid of any extra capacity
    *     INPUT  :
    *     OUTPUT :
    **************************************/
   template <typename T, typename A>
   void vector <T, A> ::shrink_to_fit()
   {
      if (numElements == numCapacity)
         return;
      else if (numElements == 0)
      {
         if (data != nullptr)
         {
            alloc.deallocate(data, numCapacity);
            data = nullptr;
            numCapacity = 0;
         }
      }
      else
      {
         // Allocate new memory with the size of numElements
         T* newData = alloc.allocate(numElements);

         // Move elements to the new memory
         for (size_t i = 0; i < numElements; ++i)
         {
            alloc.construct(&newData[i], data[i]);
            alloc.destroy(&data[i]);
         }

         // Deallocate old memory
         alloc.deallocate(data, numCapacity);

         // Update data pointer and capacity
         data = newData;
         numCapacity = numElements;
      }
   }




   /*****************************************
    * VECTOR :: SUBSCRIPT
    * Read-Write access
    ****************************************/
   template <typename T, typename A>
   T& vector <T, A> :: operator [] (size_t index)
   {
      return data[index];

   }

   /******************************************
    * VECTOR :: SUBSCRIPT
    * Read-Write access
    *****************************************/
   template <typename T, typename A>
   const T& vector <T, A> :: operator [] (size_t index) const
   {
      return data[index];
   }

   /*****************************************
    * VECTOR :: FRONT
    * Read-Write access
    ****************************************/
   template <typename T, typename A>
   T& vector <T, A> ::front()
   {
      return data[0];
   }

   /******************************************
    * VECTOR :: FRONT
    * Read-Write access
    *****************************************/
   template <typename T, typename A>
   const T& vector <T, A> ::front() const
   {
      return data[0];
   }

   /*****************************************
    * VECTOR :: FRONT
    * Read-Write access
    ****************************************/
   template <typename T, typename A>
   T& vector <T, A> ::back()
   {
      return data[numElements - 1];
   }

   /******************************************
    * VECTOR :: FRONT
    * Read-Write access
    *****************************************/
   template <typename T, typename A>
   const T& vector <T, A> ::back() const
   {
      return data[numElements - 1];
   }

   /***************************************
    * VECTOR :: PUSH BACK
    * This method will add the element 't' to the
    * end of the current buffer.  It will also grow
    * the buffer as needed to accomodate the new element
    *     INPUT  : 't' the new element to be added
    *     OUTPUT : *this
    **************************************/
   template <typename T, typename A>
   void vector <T, A> ::push_back(const T& t)
   {
      if (size() == 0) {
         reserve(1);
      }
      // Vector is at capacity: double capacity
      if (size() == capacity()) {
         reserve(capacity() * 2);
      }
      // Add t to end of current values and increment numElements
      alloc.construct(data + numElements++, t);
   }

   template <typename T, typename A>
   void vector <T, A> ::push_back(T&& t)
   {
      if (size() == 0) {
         reserve(1);
      }
      // Vector is at capacity: double capacity
      if (size() == capacity()) {
         reserve(capacity() * 2);
      }
      // Move t to end of current values and increment numElements
      alloc.construct(data + numElements++, std::move(t));
   }

   /***************************************
    * VECTOR :: SWAP
    * This method will swap the contents of the
    * rhs onto *this, growing the buffer as needed
    *     INPUT  : rhs the vector to swap with
    *     OUTPUT : *this
    **************************************/
   template <typename T, typename A>
   void vector <T, A> ::swap(vector& rhs)
   {
      // Swap data pointers
      T* tempData = data;
      data = rhs.data;
      rhs.data = tempData;

      // Swap number of elements
      size_t tempNumElements = numElements;
      numElements = rhs.numElements;
      rhs.numElements = tempNumElements;

      // Swap capacity
      size_t tempNumCapacity = numCapacity;
      numCapacity = rhs.numCapacity;
      rhs.numCapacity = tempNumCapacity;

   }

   /***************************************
    * VECTOR :: ASSIGNMENT
    * This operator will copy the contents of the
    * rhs onto *this, growing the buffer as needed
    *     INPUT  : rhs the vector to copy from
    *     OUTPUT : *this
    **************************************/
   template <typename T, typename A>
   vector <T, A>& vector <T, A> :: operator = (const vector& rhs)
   {
      if (this != &rhs)
      {
         // If the destination is smaller than the source
         if (rhs.numElements > numCapacity)
         {
            // Allocate new memory
            T* newData = alloc.allocate(rhs.numElements);

            // Copy elements from rhs to newData
            for (size_t i = 0; i < rhs.numElements; ++i)
               alloc.construct(&newData[i], rhs.data[i]);

            // Destroy existing elements
            for (size_t i = 0; i < numElements; ++i)
               alloc.destroy(&data[i]);
            alloc.deallocate(data, numCapacity);

            // Update data pointer and capacity
            data = newData;
            numCapacity = rhs.numElements;
         }
         else
         {
            // Copy elements from rhs to existing data
            for (size_t i = 0; i < rhs.numElements; ++i)
            {
               if (i < numElements)
                  data[i] = rhs.data[i];
               else
                  alloc.construct(&data[i], rhs.data[i]);
            }

            // Destroy any remaining elements in the destination
            for (size_t i = rhs.numElements; i < numElements; ++i)
               alloc.destroy(&data[i]);
         }

         // Update the number of elements
         numElements = rhs.numElements;
      }
      return *this;
   }
   template <typename T, typename A>
   vector <T, A>& vector <T, A> :: operator = (vector&& rhs)
   {
      if (this != &rhs)
      {
         // Destroy existing elements
         for (size_t i = 0; i < numElements; ++i)
            alloc.destroy(&data[i]);
         alloc.deallocate(data, numCapacity);

         // Move data from rhs to *this
         data = rhs.data;
         numElements = rhs.numElements;
         numCapacity = rhs.numCapacity;
         alloc = std::move(rhs.alloc);

         // Reset rhs
         rhs.data = nullptr;
         rhs.numElements = 0;
         rhs.numCapacity = 0;
      }
      return *this;
   }




} // namespace custom
