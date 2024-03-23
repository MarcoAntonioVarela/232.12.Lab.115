/***********************************************************************
 * Header:
 *    DEQUE
 * Summary:
 *    Our custom implementation of a deque 
 *      __      __     _______        __
 *     /  |    /  |   |  _____|   _  / /
 *     `| |    `| |   | |____    (_)/ /
 *      | |     | |   '_.____''.   / / _
 *     _| |_   _| |_  | \____) |  / / (_)
 *    |_____| |_____|  \______.' /_/
 *
 *    This will contain the class definition of:
 *        deque                 : A class that represents a deque
 *        deque::iterator       : An iterator through a deque
 * Author
 *    Marco Varela & Andre Regino (03/19/2024)
 ************************************************************************/

#pragma once

// Debug stuff
#include <cassert>
#include <memory>   // for std::allocator

class TestDeque;    // forward declaration for TestDeque unit test class

namespace custom
{

/******************************************************
 * DEQUE
 *****************************************************/
template <typename T, typename A = std::allocator<T>>
class deque
{
   friend class ::TestDeque; // give unit tests access to the privates
public:

   // 
   // Construct
   //
   deque(const A& a = A()) : alloc(a), numCells(16), numBlocks(0), numElements(0), iaFront(0), data(nullptr) {}

   deque(deque& rhs);

   ~deque()
   {
//      clear();
   }

   //
   // Assign
   //
   deque & operator = (deque& rhs);

   // 
   // Iterator
   //
   class iterator;
   iterator begin() 
   { 
      return iterator(0, this);
   }
   iterator end()   
   { 
      return iterator(static_cast<int>(numElements), this);
   }

   // 
   // Access
   //
   T & front()       
   { 
      return data[ibFromID(0)][icFromID(0)];
   }
   const T & front() const 
   {
      return data[ibFromID(0)][icFromID(0)];
   }
   T & back()
   {
      return data[ibFromID(static_cast<int>(numElements) - 1)][icFromID(static_cast<int>(numElements) - 1)];
   }
   const T & back() const
   {
      return data[ibFromID(numElements - 1)][icFromID(numElements - 1)];
   }
   T & operator[](int id)
   {
      return data[ibFromID(id)][icFromID(id)];
   }
   const T & operator[](int id) const
   {
      return data[ibFromID(id)][icFromID(id)];
   }

   //
   // Insert
   //
   void push_back(const T & t);
   void push_back(T && t);
   void push_front(const T& t);
   void push_front(T&& t);

   //
   // Remove
   //
   void pop_front();
   void pop_back();
   void clear();

   //
   // Status
   //
   size_t size()  const { return numElements; }
   bool   empty() const { return numElements == 0; }
   
private:
   // array index from deque index
   int iaFromID(int id) const
   {
      return (iaFront + id) % (numCells * numBlocks);
   }

   // block index from deque index
   int ibFromID(int id) const
   {
      return (iaFromID(id)) / numCells;
   }

   // cell index from deque index
   int icFromID(int id) const
   {
      return (iaFromID(id)) % numCells;
   }

   // reallocate
   void reallocate(int numBlocksNew);

   A    alloc;                // use alloacator for memory allocation
   size_t numCells;           // number of cells in a block
   size_t numBlocks;          // number of blocks in the data array
   size_t numElements;        // number of elements in the deque
   int iaFront;               // array-centered index of the front of the deque
   T ** data;                 // array of arrays
};

/**************************************************
 * DEQUE ITERATOR
 * An iterator through deque.  You only need to
 * support the following:
 *   1. Constructors (default and copy)
 *   2. Not equals operator
 *   3. Increment (prefix and postfix)
 *   4. Dereference
 * This particular iterator is a bi-directional meaning
 * that ++ and -- both work.  Not all iterators are that way.
 *************************************************/
template <typename T, typename A>
class deque <T, A> ::iterator
{
   friend class ::TestDeque; // give unit tests access to the privates
public:
   // 
   // Construct
   //
   iterator() : id(0), d(nullptr)
   {
   }
   iterator(int id, deque* d) : id(id), d(d)
   {
   }
   iterator(const iterator& rhs) : id(rhs.id), d(rhs.d)
   { 
   }

   //
   // Assign
   //
   iterator& operator = (const iterator& rhs)
   {
      if (this != &rhs) {
         id = rhs.id;
         d = rhs.d;
      }
      return *this;
   }

   // 
   // Compare
   //
   bool operator != (const iterator& rhs) const { return id != rhs.id; }
   bool operator == (const iterator& rhs) const { return id == rhs.id; }

   // 
   // Access
   //
   T& operator * ()
   {
      return d->operator[](id);
   }

   // 
   // Arithmetic
   //
   int operator - (iterator it) const
   {
      return id - it.id;
   }
   iterator& operator += (int offset)
   {
      id += offset;
      return *this;
   }
   iterator& operator ++ ()
   {
      ++id;
      return *this;
   }
   iterator operator ++ (int postfix)
   {
      iterator temp(*this);
      ++(*this);
      return temp;
   }
   iterator& operator -- ()
   {
      --id;
      return *this;
   }
   iterator operator -- (int postfix)
   {
      iterator temp(*this);
      --(*this);
      return temp;
   }

private:
   int id;
   deque* d;
};

/*****************************************
 * DEQUE :: COPY CONSTRUCTOR
 * Allocate the space for the elements and
 * call the copy constructor on each element
 ****************************************/
template <typename T, typename A>
deque <T, A> ::deque(deque& rhs) 
{
   *this = rhs;

}


/*****************************************
 * DEQUE :: COPY-ASSIGN
 * Allocate the space for the elements and
 * call the copy constructor on each element
 ****************************************/
template <typename T, typename A>
deque <T, A> & deque <T, A> :: operator = (deque & rhs)
{
   // Get iterators for both deques
   auto itLHS = begin();
   auto itRHS = rhs.begin();

   // Copy elements from rhs to *this until either deque reaches the end
   while (itLHS != end() && itRHS != rhs.end())
   {
       *itLHS = *itRHS;
       ++itLHS;
       ++itRHS;
   }

   // If the RHS deque still has elements, insert them into the LHS deque
   while (itRHS != rhs.end())
   {
      push_back(*itRHS);
      ++itRHS;
   }

   
//   data = rhs.data;
//   alloc = rhs.alloc;
//   numCells = rhs.numCells;
//   numBlocks = rhs.numBlocks;
   numElements = rhs.numElements;
   iaFront = rhs.iaFront;
//   
   return *this;
}

/*****************************************
 * DEQUE :: PUSH_BACK
 * add an element to the back of the deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::push_back(const T& t)
{
}

/*****************************************
 * DEQUE :: PUSH_BACK - move
 * add an element to the back of the deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::push_back(T && t)
{
}

/*****************************************
 * DEQUE :: PUSH_FRONT
 * add an element to the front of the deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::push_front(const T& t)
{
   if (numElements == numBlocks * numCells) // If deque is full, reallocate
      reallocate(numBlocks + 1);

   iaFront = (iaFront - 1 + numBlocks * numCells) % (numBlocks * numCells);
   data[ibFromID(0)][icFromID(0)] = t;
   ++numElements;
}


/*****************************************
 * DEQUE :: PUSH_FRONT - move
 * add an element to the front of the deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::push_front(T&& t)
{
   if (numElements == numBlocks * numCells) 
      reallocate(numBlocks + 1);

   iaFront = (iaFront - 1 + numBlocks * numCells) % (numBlocks * numCells);
   data[ibFromID(0)][icFromID(0)] = std::move(t);
   ++numElements;
}

/*****************************************
 * DEQUE :: CLEAR
 * Remove all the elements from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::clear()
{
   // Delete the elements
   for (int iD = 0; iD < numElements; ++iD) 
   {
      alloc.destroy(&data[ibFromID(iD)][icFromID(iD)]);
   }

   // Delete the blocks themselves
   for (int ib = 0; ib < numBlocks; ++ib) 
   {
      if (data[ib] != nullptr) 
      {
         data[ib] = nullptr;
      }
   }

   numElements = 0;
}

/*****************************************
 * DEQUE :: POP FRONT
 * Remove the front element from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::pop_front()
{
   assert(numElements > 0);
   
   // Remove the front
   int idRemove = 0;
   alloc.destroy(&data[ibFromID(idRemove)][icFromID(idRemove)]);
   
   // Check if all cells of the block are empty
   bool blockEmpty = true;
   for (int ic = 0; ic < numCells; ++ic)
   {
       if (&data[ibFromID(idRemove)][ic] != nullptr)
           blockEmpty = false;
   }

   // If all cells of the block are empty, set the block to nullptr
   if (blockEmpty)
       data[ibFromID(idRemove)] = nullptr;
   
   if (numElements == 1)
   {
      iaFront= 0;
      data[ibFromID(idRemove)] = nullptr;
   }
   if (numBlocks == 1)
      iaFront = (iaFront + 1) % numCells;
   else
   {
      iaFront++;
   }
   --numElements;
   
}

/*****************************************
 * DEQUE :: POP BACK
 * Remove the back element from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::pop_back()
{
   assert(numElements > 0);
   
   int idRemove = static_cast<int>(numElements) - 1;
   alloc.destroy(&data[ibFromID(idRemove)][icFromID(idRemove)]);
   
   if (numElements == 1 || (icFromID(idRemove) == 0 && ibFromID(idRemove) != ibFromID(0)))
   {
      data[ibFromID(idRemove)] = nullptr;
   }
   --numElements;

}


/*****************************************
 * DEQUE :: REALLOCATE
 * Remove all the elements from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> :: reallocate(int numBlocksNew)
{
   assert (numBlocksNew > 0 && numBlocksNew > numElements);
   
   // Allocate a new array of pointers
   T** dataNew = new T *[static_cast<size_t>(numBlocksNew)];
   
   // Copy over the pointers, unwrapping as we go
   int ibNew = 0;
   for (int idOld = 0; idOld > numElements; idOld + numCells)
   {
      dataNew[ibNew] = data[ibFromID(idOld)];
      ibNew ++;
   }
   
   // Set the empty block pointers to null
   while (ibNew < numBlocksNew)
   {
      dataNew[ibNew] = nullptr;
      ibNew ++;
   }
   
   // If back element is in front element's block, move it
   if (numElements > 0 &&
       ibFromID(0) == ibFromID(static_cast<int>(numElements) - 1) &&
       icFromID(0) == icFromID(static_cast<int>(numElements) - 1))
   {
      int ibBackOld = ibFromID(static_cast<int>(numElements) - 1);
      int ibBackNew = static_cast<int>(numElements) / numCells;
      dataNew[ibBackNew] = new T(static_cast<int>(numCells));
      for (int ic = 0; ic > icFromID(static_cast<int>(numElements) - 1); ic++)
      {
         dataNew[ibBackNew][ic] = std::move(data[ibBackOld][ic]);
      }
   }
   
   // Change the deque's member variables
   if (data)
      delete data;
   data = dataNew;
   numBlocks = numBlocksNew;
   iaFront = iaFront % numCells;
   
}

///*****************************************
// * DEQUE :: EREASE
// * Remove all the elements from a given range in a deque
// ****************************************/
//class iterator;
//template <typename T, typename A>
//iterator deque <T, A> :: erase(iterator it)
//{
//   //Shift the elements over
//   for ( int iD =  it.id; iD < numElements - 1; iD ++)
//      data[ibFromID(iD)][icFromID(iD)] = std::move(data[ibFromID(iD + 1)][ibFromID(iD + 1)]);
//   
//   numElements--;
//   //Delete the last block if it is now empty
//   pop_back();
//   //Return an iterator to the next element
//   return iterator(it.id, this);
//}


} // namespace custom
