/***********************************************************************
 * Header:
 *    LIST
 * Summary:
 *    Our custom implementation of std::list
 *      __      __     _______        __
 *     /  |    /  |   |  _____|   _  / /
 *     `| |    `| |   | |____    (_)/ /
 *      | |     | |   '_.____''.   / / _
 *     _| |_   _| |_  | \____) |  / / (_)
 *    |_____| |_____|  \______.' /_/
 *
 *    This will contain the class definition of:
 *        List         : A class that represents a List
 *        ListIterator : An iterator through List
 * Authors
 *    Daniel Carr, Jarom Anderson, Arlo Jolly
 ************************************************************************/

#pragma once
#include <cassert>          // for ASSERT
#include <iostream>         // for nullptr
#include <new>              // std::bad_alloc
#include <memory>           // for std::allocator
#include <initializer_list> // for std::initializer_list

class TestList; // forward declaration for unit tests
class TestHash; // forward declaration for hash used later

namespace custom
{

   /**************************************************
    * LIST
    * Just like std::list
    **************************************************/
   template <typename T, typename A = std::allocator<T>>
   class list
   {
      friend class ::TestList; // give unit tests access to the privates
      friend class ::TestHash;
      friend void swap(list& lhs, list& rhs);
   public:

      //
      // Construct
      //

      list(const A& a = A()) : numElements(0), pHead(nullptr), pTail(nullptr) {}
      list(list <T, A>& rhs, const A& a = A())
         : alloc(a), numElements(0), pHead(nullptr), pTail(nullptr)
      {
         if (rhs.pHead != nullptr)
         {
            Node* pCurrent = rhs.pHead;
            while (pCurrent != nullptr)
            {
               push_back(pCurrent->data); // Copy each node's data
               pCurrent = pCurrent->pNext;
            }
         }
      }
      list(list <T, A>&& rhs, const A& a = A());
      list(size_t num, const T& t, const A& a = A());
      list(size_t num, const A& a = A());
      list(const std::initializer_list<T>& il, const A& a = A())
         : alloc(a), numElements(0), pHead(nullptr), pTail(nullptr)
      {
         for (const T& item : il)
            push_back(item); // Copy each element from the initializer list
      }
      template <class Iterator>
      list(Iterator first, Iterator last, const A& a = A())
         : alloc(a), numElements(0), pHead(nullptr), pTail(nullptr)
      {
         for (Iterator it = first; it != last; ++it)
            push_back(*it); // Copy each element from the range
      }
      ~list()
      {
         clear(); // Clear the list, which will delete all nodes
      }

      //
      // Assign
      //

      list <T, A>& operator = (list <T, A>& rhs);
      list <T, A>& operator = (list <T, A>&& rhs);
      list <T, A>& operator = (const std::initializer_list<T>& il);
      void swap(list <T, A>& rhs)
      {
         Node* tempHead = rhs.pHead;
         rhs.pHead = pHead;
         pHead = tempHead;

         Node* tempTail = rhs.pTail;
         rhs.pTail = pTail;
         pTail = tempTail;

         size_t tempElements = rhs.numElements;
         rhs.numElements = numElements;
         numElements = tempElements;
      }

      //
      // Iterator
      //

      class iterator;
      iterator begin() { return iterator(pHead); }
      iterator rbegin() { return iterator(pTail); }
      iterator end() { return iterator(nullptr); }

      //
      // Access
      //

      T& front();
      T& back();

      //
      // Insert
      //

      void push_front(const T& data);
      void push_front(T&& data);
      void push_back(const T& data);
      void push_back(T&& data);
      iterator insert(iterator it, const T& data);
      iterator insert(iterator it, T&& data);

      //
      // Remove
      //

      void pop_back();
      void pop_front();
      void clear();
      iterator erase(const iterator& it);

      //
      // Status
      //

      bool empty()  const { return pHead == nullptr; }
      size_t size() const { return numElements; }

   private:
      // nested linked list class
      class Node;

      // member variables
      A    alloc;         // use alloacator for memory allocation
      size_t numElements; // though we could count, it is faster to keep a variable
      Node* pHead;       // pointer to the beginning of the list
      Node* pTail;       // pointer to the ending of the list
   };

   /*************************************************
    * NODE
    * the node class.  Since we do not validate any
    * of the setters, there is no point in making them
    * private.  This is the case because only the
    * List class can make validation decisions
    *************************************************/
   template <typename T, typename A>
   class list <T, A> ::Node
   {
   public:
      //
      // Construct
      //
      Node() : pNext(nullptr), pPrev(nullptr) {}
      Node(const T& data) : pNext(nullptr), pPrev(nullptr), data(data) {}
      Node(T&& data) : pNext(nullptr), pPrev(nullptr), data(std::move(data)) {}

      //
      // Member Variables
      //

      T data;             // user data
      Node* pNext;       // pointer to next node
      Node* pPrev;       // pointer to previous node
   };

   /*************************************************
    * LIST ITERATOR
    * Iterate through a List, non-constant version
    ************************************************/
   template <typename T, typename A>
   class list <T, A> ::iterator
   {
      friend class ::TestList; // give unit tests access to the privates
      friend class ::TestHash;
      template <typename TT, typename AA>
      friend class custom::list;

   public:
      // constructors, destructors, and assignment operator
      iterator() : p(nullptr) {}
      iterator(Node* pRHS) : p(pRHS) {}
      iterator(const iterator& rhs) : p(rhs.p) {}
      iterator& operator = (const iterator& rhs)
      {
         if (this != &rhs)
            p = rhs.p;
         return *this;
      }

      // equals, not equals operator
      bool operator == (const iterator& rhs) const { return p == rhs.p; }
      bool operator != (const iterator& rhs) const { return p != rhs.p; }

      // dereference operator, fetch a node
      T& operator * () { return p->data; }

      // postfix increment
      iterator operator ++ (int)
      {
         iterator tmp = *this; p = p->pNext; return tmp;
      }

      // prefix increment
      iterator& operator ++ () { p = p->pNext; return *this; }

      // postfix decrement
      iterator operator -- (int)
      {
         iterator tmp = *this; p = p->pPrev; return tmp;
      }

      // prefix decrement
      iterator& operator -- ()
      {
         p = p->pPrev; return *this;
      }

      // two friends who need to access p directly
      friend iterator list <T, A> ::insert(iterator it, const T& data);
      friend iterator list <T, A> ::insert(iterator it, T&& data);
      friend iterator list <T, A> ::erase(const iterator& it);

   private:

      typename list <T, A> ::Node* p;
   };

   /*****************************************
    * LIST :: NON-DEFAULT constructors
    * Create a list initialized to a value
    ****************************************/
   template <typename T, typename A>
   list <T, A> ::list(size_t num, const T& t, const A& a)
      : alloc(a), numElements(0), pHead(0), pTail(0)
   {
      if (num > 0)
      {
         pHead = new Node(t); // Copy constructor should be called here
         Node* pCurrent = pHead;
         for (size_t i = 1; i < num; ++i)
         {
            Node* pNew = new Node(t); // Copy constructor should be called here
            pCurrent->pNext = pNew;
            pNew->pPrev = pCurrent;
            pCurrent = pNew;
         }
         pTail = pCurrent;
         numElements = num;
      }
   }

   /*****************************************
    * LIST :: NON-DEFAULT constructors
    * Create a list initialized to a value
    ****************************************/
   template <typename T, typename A>
   list <T, A> ::list(size_t num, const A& a)
      : alloc(a), numElements(0), pHead(0), pTail(0)
   {
      if (num)
      {
         list <T, A> ::Node* pPrevious;
         list <T, A> ::Node* pNew;
         pHead = pPrevious = pNew = new list <T, A> ::Node();
         pHead->pPrev = nullptr;
         for (size_t i = 1; i < num; i++)
         {
            pNew = new list <T, A> ::Node();
            pNew->pPrev = pPrevious;
            pNew->pPrev->pNext = pNew;
            pPrevious = pNew;
         }
         pNew->pNext = nullptr;
         pTail = pNew;
         numElements = num;
      }
   }

   /*****************************************
    * LIST :: MOVE constructors
    * Steal the values from the RHS
    ****************************************/
   template <typename T, typename A>
   list <T, A> ::list(list <T, A>&& rhs, const A& a) :
      numElements(rhs.numElements), pHead(rhs.pHead), pTail(rhs.pTail), alloc(a)
   {
      rhs.pHead = rhs.pTail = nullptr;
      rhs.numElements = 0;
   }

   /**********************************************
    * LIST :: assignment operator - MOVE
    * Copy one list onto another
    *     INPUT  : a list to be moved
    *     OUTPUT :
    *     COST   : O(n) with respect to the size of the LHS
    *********************************************/
   template <typename T, typename A>
   list <T, A>& list <T, A> :: operator = (list <T, A>&& rhs)
   {
      clear();
      swap(rhs);
      return *this;
   }

   /**********************************************
    * LIST :: assignment operator
    * Copy one list onto another
    *     INPUT  : a list to be copied
    *     OUTPUT :
    *     COST   : O(n) with respect to the number of nodes
    *********************************************/
   template <typename T, typename A>
   list <T, A>& list <T, A> :: operator = (list <T, A>& rhs)
   {
      iterator itRHS = rhs.begin();
      iterator itLHS = begin();
      while (itRHS != rhs.end() && itLHS != end())
      {
         *itLHS = *itRHS;
         ++itRHS;
         ++itLHS;
      }

      if (itRHS != rhs.end())
      {
         while (itRHS != rhs.end())
         {
            push_back(*itRHS);
            ++itRHS;
         }
      }
      else if (rhs.empty())
      {
         clear();
      }
      else if (itLHS != end())
      {
         Node* p = itLHS.p;
         pTail = p->pPrev;
         Node* pNext = p->pNext;
         while (p)
         {
            pNext = p->pNext;
            delete p;
            p = pNext;
            numElements--;
         }
         pTail->pNext = nullptr;
      }

      return *this;
   }

   /**********************************************
    * LIST :: assignment operator
    * Copy one list onto another
    *     INPUT  : a list to be copied
    *     OUTPUT :
    *     COST   : O(n) with respect to the number of nodes
    *********************************************/
   template <typename T, typename A>
   list <T, A>& list <T, A> :: operator =
      (const std::initializer_list<T>& rhs)
   {
      typename std::initializer_list<T>::const_iterator itRHS = rhs.begin();
      iterator itLHS = begin();
      while (itRHS != rhs.end() && itLHS != end())
      {
         *itLHS = *itRHS;
         ++itRHS;
         ++itLHS;
      }

      if (itRHS != rhs.end())
      {
         while (itRHS != rhs.end())
         {
            push_back(*itRHS);
            ++itRHS;
         }
      }
      else if (numElements > rhs.size()) // Initalizer list has no empty()
      {
         iterator it = begin();
         size_t i = 0;
         while (i < rhs.size() && it != end())
         {
            ++it;
            ++i;
         }

         while (it != end())
         {
            it = erase(it);
         }
      }

      return *this;
   }

   /**********************************************
    * LIST :: CLEAR
    * Remove all the items currently in the linked list
    *     INPUT  :
    *     OUTPUT :
    *     COST   : O(n) with respect to the number of nodes
    *********************************************/
   template <typename T, typename A>
   void list <T, A> ::clear()
   {
      while (pHead != nullptr)
      {
         Node* pDelete = pHead;
         pHead = pHead->pNext;
         delete pDelete;
      }
      pTail = nullptr;
      numElements = 0;
   }

   /*********************************************
    * LIST :: PUSH BACK
    * add an item to the end of the list
    *    INPUT  : data to be added to the list
    *    OUTPUT :
    *    COST   : O(1)
    *********************************************/
   template <typename T, typename A>
   void list <T, A> ::push_back(const T& data)
   {
      Node* pNew = new Node(data);
      if (pTail != nullptr)
      {
         pTail->pNext = pNew;
         pNew->pPrev = pTail;
         pTail = pNew;
      }
      else
         pHead = pTail = pNew;
      numElements++;
   }

   template <typename T, typename A>
   void list <T, A> ::push_back(T&& data)
   {
      Node* pNew = new Node(std::move(data));
      if (pTail != nullptr)
      {
         pTail->pNext = pNew;
         pNew->pPrev = pTail;
         pTail = pNew;
      }
      else
         pHead = pTail = pNew;
      numElements++;

   }

   /*********************************************
    * LIST :: PUSH FRONT
    * add an item to the head of the list
    *     INPUT  : data to be added to the list
    *     OUTPUT :
    *     COST   : O(1)
    *********************************************/
   template <typename T, typename A>
   void list <T, A> ::push_front(const T& data)
   {
      Node* pNew = new Node(data);
      if (pHead != nullptr)
      {
         pHead->pPrev = pNew;
         pNew->pNext = pHead;
         pHead = pNew;
      }
      else
         pHead = pTail = pNew;
      numElements++;
   }

   template <typename T, typename A>
   void list <T, A> ::push_front(T&& data)
   {
      Node* pNew = new Node(std::move(data));
      if (pHead != nullptr)
      {
         pHead->pPrev = pNew;
         pNew->pNext = pHead;
         pHead = pNew;
      }
      else
         pHead = pTail = pNew;
      numElements++;
   }


   /*********************************************
    * LIST :: POP BACK
    * remove an item from the end of the list
    *    INPUT  :
    *    OUTPUT :
    *    COST   : O(1)
    *********************************************/
   template <typename T, typename A>
   void list <T, A> ::pop_back()
   {
      if (pTail != nullptr)
      {
         Node* pDelete = pTail;
         pTail = pTail->pPrev;
         if (pTail != nullptr)
            pTail->pNext = nullptr;
         else
            pHead = nullptr;
         delete pDelete;
         numElements--;
      }
   }

   /*********************************************
    * LIST :: POP FRONT
    * remove an item from the front of the list
    *    INPUT  :
    *    OUTPUT :
    *    COST   : O(1)
    *********************************************/
   template <typename T, typename A>
   void list <T, A> ::pop_front()
   {
      if (pHead != nullptr)
      {
         Node* pDelete = pHead;
         pHead = pHead->pNext;
         if (pHead != nullptr)
            pHead->pPrev = nullptr;
         else
            pTail = nullptr;
         delete pDelete;
         numElements--;
      }
   }

   /*********************************************
    * LIST :: FRONT
    * retrieves the first element in the list
    *     INPUT  :
    *     OUTPUT : data to be displayed
    *     COST   : O(1)
    *********************************************/
   template <typename T, typename A>
   T& list <T, A> ::front()
   {
      if (numElements != 0)
      {
         return pHead->data;
      }
      else
         return *(new T);
   }

   /*********************************************
    * LIST :: BACK
    * retrieves the last element in the list
    *     INPUT  :
    *     OUTPUT : data to be displayed
    *     COST   : O(1)
    *********************************************/
   template <typename T, typename A>
   T& list <T, A> ::back()
   {
      if (numElements != 0)
      {
         return pTail->data;
      }
      else
         return *(new T);
   }


   /******************************************
    * LIST :: REMOVE
    * remove an item from the middle of the list
    *     INPUT  : an iterator to the item being removed
    *     OUTPUT : iterator to the new location
    *     COST   : O(1)
    ******************************************/
   template <typename T, typename A>
   typename list <T, A> ::iterator  list <T, A> ::
      erase(const list <T, A> ::iterator& it)
   {
      Node* pDelete = it.p;
      if (pDelete == nullptr)
         return end();

      if (pDelete->pPrev != nullptr)
         pDelete->pPrev->pNext = pDelete->pNext;
      else
         pHead = pDelete->pNext;

      if (pDelete->pNext != nullptr)
         pDelete->pNext->pPrev = pDelete->pPrev;
      else
         pTail = pDelete->pPrev;

      iterator next(pDelete->pNext);
      delete pDelete;
      numElements--;
      return next;
   }

   /******************************************
    * LIST :: INSERT
    * add an item to the middle of the list
    *     INPUT  : data to be added to the list
    *              an iterator to the location where it is to be inserted
    *     OUTPUT : iterator to the new item
    *     COST   : O(1)
    ******************************************/
   template <typename T, typename A>
   typename list <T, A> ::iterator list <T, A> ::
      insert(list <T, A> ::iterator it,
         const T& data)
   {
      Node* pNew = new Node(data);
      if (this->empty())
      {
         pHead = pTail = pNew;
         numElements = 1;
         return this->begin();
      }
      else if (it == end())
      {
         pTail->pNext = pNew;
         pNew->pPrev = pTail;
         pTail = pNew;
         numElements++;
         return iterator(pNew);
      }
      else if (it != end())
      {
         pNew->pPrev = it.p->pPrev;
         pNew->pNext = it.p;

         if (pNew->pPrev)
            pNew->pPrev->pNext = pNew;
         else
            pHead = pNew;

         if (pNew->pNext)
            pNew->pNext->pPrev = pNew;
         else
            pTail = pNew;

         numElements++;
         return iterator(pNew);
      }
      else
         return end();
   }


   /******************************************
    * LIST :: INSERT
    * add several items into the middle of the list
    *     INPUT  : data to be added to the list
    *              an iterator to the location where it is to be inserted
    *     OUTPUT : iterator to the new item
    *     COST   : O(1)
    ******************************************/
   template <typename T, typename A>
   typename list <T, A> ::iterator list <T, A> ::
      insert(list <T, A> ::iterator it,
         T&& data)
   {
      Node* pNew = new Node(std::move(data));
      if (this->empty())
      {
         pHead = pTail = pNew;
         numElements = 1;
         return this->begin();
      }
      else if (it == end())
      {
         pTail->pNext = pNew;
         pNew->pPrev = pTail;
         pTail = pNew;
         numElements++;
         return iterator(pNew);
      }
      else if (it != end())
      {
         pNew->pPrev = it.p->pPrev;
         pNew->pNext = it.p;

         if (pNew->pPrev)
            pNew->pPrev->pNext = pNew;
         else
            pHead = pNew;

         if (pNew->pNext)
            pNew->pNext->pPrev = pNew;
         else
            pTail = pNew;

         numElements++;
         return iterator(pNew);
      }
      else
         return end();
   }

   /**********************************************
    * LIST :: assignment operator - MOVE
    * Copy one list onto another
    *     INPUT  : a list to be moved
    *     OUTPUT :
    *     COST   : O(n) with respect to the size of the LHS
    *********************************************/
   template <typename T, typename A>
   void swap(list <T, A>& lhs, list <T, A>& rhs)
   {
      list <T, A>* pTemp = lhs;
      lhs = rhs;
      rhs = pTemp;
   }

}; // namespace custom
