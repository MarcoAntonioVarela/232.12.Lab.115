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
      clear();
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
      return iterator(numElements, this);
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
      return data[ibFromID(numElements - 1)][icFromID(numElements - 1)];
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
      return (iaFront + id) % numCells;
   }

   // block index from deque index
   int ibFromID(int id) const
   {
      return (iaFront + id) / numCells;
   }

   // cell index from deque index
   int icFromID(int id) const
   {
      return (iaFront + id) % numCells;
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
   alloc = rhs.alloc;
   numCells = rhs.numCells;
   numBlocks = rhs.numBlocks;
   numElements = rhs.numElements;
   iaFront = rhs.iaFront;

   // Allocate memory for data array
   data = new T * [numBlocks];
   for (size_t i = 0; i < numBlocks; ++i) {
      data[i] = new T[numCells];
      for (size_t j = 0; j < numCells; ++j) {
         data[i][j] = rhs.data[i][j]; // Call copy constructor of T
      }
   }
}

/*****************************************
 * DEQUE :: COPY-ASSIGN
 * Allocate the space for the elements and
 * call the copy constructor on each element
 ****************************************/
template <typename T, typename A>
deque <T, A> & deque <T, A> :: operator = (deque & rhs)
{
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
}

/*****************************************
 * DEQUE :: PUSH_FRONT - move
 * add an element to the front of the deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::push_front(T&& t)
{
}

/*****************************************
 * DEQUE :: CLEAR
 * Remove all the elements from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::clear()
{
}

/*****************************************
 * DEQUE :: POP FRONT
 * Remove the front element from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::pop_front()
{
}

/*****************************************
 * DEQUE :: POP BACK
 * Remove the back element from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> ::pop_back()
{
   assert(numElements > 0);
   alloc.destroy(&data[ibFromID(numElements - 1)][icFromID(numElements - 1)]);
   --numElements;
}


/*****************************************
 * DEQUE :: REALLOCATE
 * Remove all the elements from a deque
 ****************************************/
template <typename T, typename A>
void deque <T, A> :: reallocate(int numBlocksNew)
{
}




} // namespace custom
