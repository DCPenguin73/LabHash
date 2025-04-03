/***********************************************************************
 * Header:
 *    HASH
 * Summary:
 *    Our custom implementation of std::unordered_Set
 *      __      __     _______        __
 *     /  |    /  |   |  _____|   _  / /
 *     `| |    `| |   | |____    (_)/ /
 *      | |     | |   '_.____''.   / / _
 *     _| |_   _| |_  | \____) |  / / (_)
 *    |_____| |_____|  \______.' /_/
 *
 *    This will contain the class definition of:
 *        unordered_set           : A class that represents a hash
 *        unordered_set::iterator : An interator through hash
 * Author
 *    Daniel Carr, Jarom ANderson, Arlo Jolly
 ************************************************************************/

#pragma once

#include "list.h"     // because this->buckets[0] is a list
#include "vector.h"   // because this->buckets is a vector
#include <memory>     // for std::allocator
#include <functional> // for std::hash
#include <cmath>      // for std::ceil


class TestHash;             // forward declaration for Hash unit tests

namespace custom
{
/************************************************
 * UNORDERED SET
 * A set implemented as a hash
 ************************************************/
template <typename T,
          typename Hash = std::hash<T>,
          typename EqPred = std::equal_to<T>,
          typename A = std::allocator<T> >
class unordered_set
{
   friend class ::TestHash;   // give unit tests access to the privates
public:
   //
   // Construct
   //
   unordered_set() :numElements(0), maxLoadFactor(1.0), buckets(8)
   {
   }
   unordered_set(size_t numBuckets) : numElements(0), maxLoadFactor(1.0), buckets(numBuckets)
   {
   }
   unordered_set(const unordered_set&  rhs)
   {
       *this = rhs;
   }
   unordered_set(unordered_set&& rhs)
   {
       *this = std::move(rhs);
   }
   template <class Iterator>
   unordered_set(Iterator first, Iterator last)
   {
      reserve(last - first);
      maxLoadFactor = 1.0;
      for (auto it = first; it < last; it++)
         insert(*it);
   }

   //
   // Assign
   //
   unordered_set& operator=(const unordered_set& rhs)
   {
      numElements = rhs.numElements;
      maxLoadFactor = rhs.maxLoadFactor;
      buckets = rhs.buckets;
      return *this;
   }
   unordered_set& operator=(unordered_set&& rhs)
   {
      numElements = std::move(rhs.numElements);
      maxLoadFactor = std::move(rhs.maxLoadFactor);
      buckets = std::move(rhs.buckets);

      rhs.numElements = 0;
      rhs.maxLoadFactor = 1.0;
      rhs.buckets.resize(8);

      return *this;
   }
   unordered_set& operator=(const std::initializer_list<T>& il)
   {
      clear();
      reserve(il.size());
      for (auto element : il)
         insert(element);
      return *this;
   }
   void swap(unordered_set& rhs)
   {
      using std::swap;
      swap(numElements, rhs.numElements);
      swap(maxLoadFactor, rhs.maxLoadFactor);
      swap(buckets, rhs.buckets);
   }

   //
   // Iterator
   //
   class iterator;
   class local_iterator;
   iterator begin()
   {
      for (auto itBucket = buckets.begin(); itBucket != buckets.end(); itBucket++)
      {
         if (!(*itBucket).empty())
         {
            return iterator(buckets.end(), itBucket, (*itBucket).begin());
         }
      }
       return end();
      // return iterator();
   }
   iterator end()
   {
      return iterator(buckets.end(), buckets.end(), buckets[0].end());
   }
   local_iterator begin(size_t iBucket)
   {
      return local_iterator(buckets[iBucket].begin());
   }
   local_iterator end(size_t iBucket)
   {
      return local_iterator(buckets[iBucket].end());
   }

   //
   // Access
   //
   size_t bucket(const T& t)
   {
      return Hash()(t) % bucket_count();
   }
   iterator find(const T& t);

   //
   // Insert
   //
   custom::pair<iterator, bool> insert(const T& t);
   void insert(const std::initializer_list<T> & il);
   void rehash(size_t numBuckets);
   void reserve(size_t num)
   {
      rehash(min_buckets_required(num));
   }

   //
   // Remove
   //
   void clear() noexcept
   {
      for (auto& bucket : buckets)
         bucket.clear();
      numElements = 0;
   }
   iterator erase(const T& t);

   //
   // Status
   //
   size_t size() const
   {
      return numElements;
   }
   bool empty() const
   {
      return size() == 0;
   }
   size_t bucket_count() const
   {
      return buckets.size();
   }
   size_t bucket_size(size_t i) const
   {
      return buckets[i].size();
   }
   float load_factor() const noexcept
   {
      return size() / bucket_count();
   }
   float max_load_factor() const noexcept
   {
      return maxLoadFactor;
   }
   void  max_load_factor(float m)
   {
      maxLoadFactor = m;
   }

private:

   size_t min_buckets_required(size_t num) const
   {
      return (size_t)std::ceil(num / maxLoadFactor);
   }

   custom::vector<custom::list<T,A>> buckets;  // each bucket in the hash
   int numElements;                            // number of elements in the Hash
   float maxLoadFactor;                        // the ratio of elements to buckets signifying a rehash
};


/************************************************
 * UNORDERED SET ITERATOR
 * Iterator for an unordered set
 ************************************************/
template <typename T, typename H, typename E, typename A>
class unordered_set <T, H, E, A> ::iterator
{
   friend class ::TestHash;   // give unit tests access to the privates
   template <typename TT, typename HH, typename EE, typename AA>
   friend class custom::unordered_set;
public:
   //
   // Construct
   iterator() : itVectorEnd(), itVector(), itList()
   {
   }
   iterator(const typename custom::vector<custom::list<T> >::iterator& itVectorEnd,
            const typename custom::vector<custom::list<T> >::iterator& itVector,
            const typename custom::list<T>::iterator &itList)
   : itVectorEnd(itVectorEnd), itVector(itVector), itList(itList)
   {
   }
   iterator(const iterator& rhs) : itVectorEnd(rhs.itVectorEnd), itVector(rhs.itVector), itList(rhs.itList)
   {
   }

   //
   // Assign
   //
   iterator& operator = (const iterator& rhs)
   {
      itVectorEnd = rhs.itVectorEnd;
      itList = rhs.itList;
      itVector = rhs.itVector;
      return *this;
   }

   //
   // Compare
   //
   bool operator != (const iterator& rhs) const
   {
      return (
         (itVectorEnd != rhs.itVectorEnd) ||
         (itVector != rhs.itVector) ||
         (itList != rhs.itList)
      );
   }
   bool operator == (const iterator& rhs) const
   {
      return (
         (itVectorEnd == rhs.itVectorEnd) &&
         (itVector == rhs.itVector) &&
         (itList == rhs.itList)
      );
   }

   //
   // Access
   //
   T& operator * ()
   {
      return *itList;
   }

   //
   // Arithmetic
   //
   iterator& operator ++ ();
   iterator operator ++ (int postfix)
   {
      iterator temp = *this;
      ++postfix;
      return temp;
   }

private:
   typename vector<list<T>>::iterator itVectorEnd;
   typename list<T>::iterator itList;
   typename vector<list<T>>::iterator itVector;
};


/************************************************
 * UNORDERED SET LOCAL ITERATOR
 * Iterator for a single bucket in an unordered set
 ************************************************/
template <typename T, typename H, typename E, typename A>
class unordered_set <T, H, E, A> ::local_iterator
{
   friend class ::TestHash;   // give unit tests access to the privates

   template <typename TT, typename HH, typename EE, typename AA>
   friend class custom::unordered_set;
public:
   //
   // Construct
   //
   local_iterator() : itList()
   {
   }
   local_iterator(const typename custom::list<T>::iterator& itList) : itList(itList)
   {
   }
   local_iterator(const local_iterator& rhs) : itList(rhs.itList)
   {
   }

   //
   // Assign
   //
   local_iterator& operator = (const local_iterator& rhs)
   {
      if (rhs.itList != itList)
         itList = rhs.itList;
      return *this;
   }

   //
   // Compare
   //
   bool operator != (const local_iterator& rhs) const
   {
      return (rhs.itList != itList);
   }
   bool operator == (const local_iterator& rhs) const
   {
      return (rhs.itList == itList);
   }

   //
   // Access
   //
   T& operator * ()
   {
      return *itList;
   }

   //
   // Arithmetic
   //
   local_iterator& operator ++ ()
   {
      ++itList;
      return *this;
   }
   local_iterator operator ++ (int postfix)
   {
      return *this;
   }

private:
   typename list<T>::iterator itList;
};


/*****************************************
 * UNORDERED SET :: ERASE
 * Remove one element from the unordered set
 ****************************************/
template <typename T, typename Hash, typename E, typename A>
typename unordered_set <T, Hash, E, A> ::iterator unordered_set<T,Hash,E,A>::erase(const T& t)
{
   /*itErase = find(t);
   if (itErase == end())
      return itErase;
   itReturn = itErase;
   itReturn++;
   itErase.itVector.erase(itErase.itList);
   numElements--;
   return itReturn;*/
   return iterator();
}

/*****************************************
 * UNORDERED SET :: INSERT
 * Insert one element into the hash
 ****************************************/
template <typename T, typename H, typename E, typename A>
custom::pair<typename custom::unordered_set<T, H, E, A>::iterator, bool> unordered_set<T, H, E, A>::insert(const T& t)
{
   /*if (find(t) != end())
      return { end(), false };

   if (load_factor() >= max_load_factor)
      rehash(bucket_count() * 2);

   size_t iBucket = bucket(t);
   buckets[iBucket].push_back(t);
   numElements++;
   return { iterator(buckets.end(), buckets.begin() + iBucket, --buckets[iBucket].end()), true };*/
   return custom::pair<custom::unordered_set<T, H, E, A>::iterator, bool>(iterator(), true);
}
template <typename T, typename H, typename E, typename A>
void unordered_set<T, H, E, A>::insert(const std::initializer_list<T> & il)
{
}

/*****************************************
 * UNORDERED SET :: REHASH
 * Re-Hash the unordered set by numBuckets
 ****************************************/
template <typename T, typename Hash, typename E, typename A>
void unordered_set<T, Hash, E, A>::rehash(size_t numBuckets)
{
   if (numBuckets <= bucket_count())
      return; // Don't rehash to a smaller size

   custom::vector<custom::list<T, A>> newBuckets(numBuckets);

   // Reinsert all elements into new buckets
   for (auto& bucket : buckets)
   {
      for (auto it = bucket.begin(); it != bucket.end(); ++it)
      {
         size_t newIndex = Hash()(*it) % numBuckets; // Compute new bucket index
         newBuckets[newIndex].push_back(std::move(*it)); // Move element into new bucket
      }
   }

   // Assign new bucket structure
   buckets = std::move(newBuckets);
}


/*****************************************
 * UNORDERED SET :: FIND
 * Find an element in an unordered set
 ****************************************/
template <typename T, typename H, typename E, typename A>
typename unordered_set <T, H, E, A> ::iterator unordered_set<T, H, E, A>::find(const T& t)
{
   // Identify bucket number corresponding to "t"
   size_t iBucket = bucket(t);

   // Get a list iterator to the element using the list’s find() method.
   typename custom::list<T, A>::iterator itList = buckets[iBucket].find(t);

   // Create an iterator to return
   if (itList != buckets[iBucket].end())
      return iterator(
         buckets.end(),
         typename custom::vector<custom::list<T, A>>::iterator(iBucket, buckets),
         itList
      );

   return end();
}

/*****************************************
 * UNORDERED SET :: ITERATOR :: INCREMENT
 * Advance by one element in an unordered set
 ****************************************/
template <typename T, typename H, typename E, typename A>
typename unordered_set <T, H, E, A> ::iterator & unordered_set<T, H, E, A>::iterator::operator ++ ()
{

   // Only advance if we are not at the end
   if (itVector == itVectorEnd)
      return *this;

   // Advance the list iterator. If we are not at the end, we are done.
   ++itList;
   if (itList != (*itVector).end())
      return *this;

   // We are at the end of the list. Find the next bucket.
   ++itVector;
   while (itVector != itVectorEnd && (*itVector).empty())
      ++itVector;
   if (itVector != itVectorEnd)
      itList = (*itVector).begin();

   return *this;

}

/*****************************************
 * SWAP
 * Stand-alone unordered set swap
 ****************************************/
template <typename T, typename H, typename E, typename A>
void swap(unordered_set<T,H,E,A>& lhs, unordered_set<T,H,E,A>& rhs)
{
   lhs.swap(rhs);
}

}
