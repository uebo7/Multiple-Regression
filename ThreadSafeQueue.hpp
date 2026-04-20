/*
  Author     : Adrian Olweiler & Logan Kaufman
  Course     : CMSC 476
  Description: A thread-safe queue template.
*/

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template<typename T>
class ThreadSafeQueue
{
public:
  ThreadSafeQueue () = default;

  // Push an element, notifying any consumers.
  // Move the value into the underlying queue.
  void
  push (T value)
  {
    // TODO
    {
      std::lock_guard<std::mutex> lock (m_mutex);
      m_elements.push (std::move (value));
    }
    m_cv.notify_one ();
  }

  // Wait until an element is available, then pop it
  //   via the reference parameter.
  // Move the element out of the underlying queue.
  void
  waitAndPop (T& value)
  {
    // TODO
    std::unique_lock<std::mutex> lock (m_mutex);
    m_cv.wait (lock, [this] { return !m_elements.empty (); });

    value = std::move (m_elements.front ());
    m_elements.pop ();
  }

  // Return whether the underlying queue is empty.
  bool
  empty () const
  {
    // TODO
    std::lock_guard<std::mutex> lock (m_mutex);
    return m_elements.empty ();
  }

private:
  // Do NOT modify these data members.
  std::queue<T> m_elements;
  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
};