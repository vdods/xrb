// ///////////////////////////////////////////////////////////////////////////
// xrb_circularqueue.hpp by Victor Dods, created 2005/02/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_CIRCULARQUEUE_HPP_)
#define _XRB_CIRCULARQUEUE_HPP_

#include "xrb.hpp"

namespace Xrb
{

/** Template arguments are the type of queue data and queue length.
  * Adding items happens strictly by enqueueing them at the end, while
  * removing items happens be dequeueing them from the beggining.  Random,
  * read-only access (index-based) is also available.
  *
  * The template argument for entry type ( @c T ) must be assignable.
  * Dequeue and Clear don't actually change the entries at all, so the
  * entries will not be destructed in any reliable way.  @c T should not
  * count on being destructed.
  *
  * @brief Templatized circular queue object.
  */
template <typename T, Uint32 queue_size>
class CircularQueue
{
public:

    /** Initializes an empty queue.
      * @brief Default constructor.
      */
    CircularQueue ();
    /** @brief Destructor.
      */
    ~CircularQueue ();

    /** The head of the queue always has index 0.
      * @brief Returns the entry in the relative position given by @c index.
      */
    T const &GetEntry (Uint32 const index) const;
    /** The size of the queue will always be <= @c queue_size.
      * @brief Returns the number of entries in this queue.
      */
    inline Uint32 GetEntryCount () const { return m_entry_count; }
    /** @brief Returns true iff the queue is empty.
      */
    inline bool IsEmpty () const { return m_entry_count == 0; }
    /** @brief Returns true iff the queue is at maximum capacity.
      */
    inline bool IsFull () const { return m_entry_count == queue_size; }

    /** Will assert if the queue is currently full.
      * @brief Enqueues the given entry at the end of the queue.
      */
    void Enqueue (T const &entry);
    /** Doesn't actually change the contents of the queue (which is important
      * if @c T relies on being destructed).
      *
      * Will assert if the queue is currently empty.
      * 
      * @brief Dequeue the head entry, returning it by value.
      */
    T Dequeue ();
    /** Doesn't actually change the contents of the queue (which is important
      * if @c T relies on being destructed).
      * @brief Clear all entries in the queue, leaving an empty queue.
      */
    void Clear ();

private:

    inline Uint32 GetIncrementedIndex (Uint32 const index)
    {
        return (index + 1) % queue_size;
    }

    // the array which stores the queue
    T m_queue[queue_size];
    // the head index of the queue
    Uint32 m_head;
    // the tail index of the queue
    Uint32 m_tail;
    // the number of entries currently in the queue
    Uint32 m_entry_count;
}; // end of class CircularQueue

// the implementation for CircularQueue
#include "xrb_circularqueue.tcpp"

} // end of namespace Xrb

#endif // !defined(_XRB_CIRCULARQUEUE_HPP_)

