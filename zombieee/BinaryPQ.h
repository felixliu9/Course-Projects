// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#ifndef BINARYPQ_H
#define BINARYPQ_H

#include <algorithm>
#include "Eecs281PQ.h"

// A specialized version of the priority queue ADT implemented as a binary
// heap.
template <typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty PQ with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{comp}
    {
        
    } // BinaryPQ

    // Description: Construct a PQ out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template <typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{comp}
    {
        data.push_back(*start);
        for(InputIterator it = start; it!= end; ++it){
            data.push_back(*it);
        }
        updatePriorities();
    } // BinaryPQ

    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ()
    {
    } // ~BinaryPQ()

    // Description: Assumes that all elements inside the heap are out of
    //              order and 'rebuilds' the heap by fixing the heap
    //              invariant.
    // Runtime: O(n)
    virtual void updatePriorities()
    {
        for(size_t i=1; i<data.size(); i++) {
            fix_up(i);
        }
    } // updatePriorities()

    // Description: Add a new element to the PQ.
    // Runtime: O(log(n))
    virtual void push(const TYPE &val)
    {
        if(data.size()==0){
            data.push_back(val);
        }
        data.push_back(val);
        fix_up(data.size() - 1);
    } // push()

    // Description: Remove the most extreme (defined by 'compare') element
    //              from the PQ.
    // Note: We will not run tests on your code that would require it to pop
    //       an element when the PQ is empty. Though you are welcome to if
    //       you are familiar with them, you do not need to use exceptions in
    //       this project.
    // Runtime: O(log(n))
    virtual void pop()
    {
        if (empty())
        {
            return;
        }
        data[1] = data.back();
        data.pop_back();
        fix_down(1);
    } // pop()

    // Description: Return the most extreme (defined by 'compare') element of
    //              the PQ. This should be a reference for speed. It MUST
    //              be const because we cannot allow it to be modified, as
    //              that might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const
    {
        return data[1];
    } // top()

    // Description: Get the number of elements in the PQ.
    // Runtime: O(1)
    virtual std::size_t size() const
    {
        return data.size() - 1;
    } // size()

    // Description: Return true if the PQ is empty.
    // Runtime: O(1)
    virtual bool empty() const
    {
        return data.empty() || data.size() == 1;
    } // empty()

private:
    // Note: This vector *must* be used for your PQ implementation.
    std::vector<TYPE> data;
    // NOTE: You are not allowed to add any member variables. You don't need
    //       a "heapSize", since you can call your own size() member
    //       function, or check data.size().
    void fix_down(size_t index)
    {
        size_t heap_size = data.size() - 1;
        while (2 * index <= heap_size)
        {
            // find larger child
            size_t larger_child_index = 2 * index;

            if (larger_child_index < heap_size &&
                this->compare(data[larger_child_index], data[larger_child_index + 1]))
            {
                ++larger_child_index;
            }

            // node at index bigger than both children, ok to return
            if (this->compare(data[larger_child_index], data[index]))
            {
                break;
            }

            std::swap(data[index], data[larger_child_index]);
            index = larger_child_index;
        }
    } // fix_down

    void fix_up(size_t index)
    {
        while (index > 1 && this->compare(data[index / 2], data[index]))
        {
            std::swap(data[index], data[index / 2]);
            index /= 2;
        }
    }
}; // BinaryPQ

#endif // BINARYPQ_H
