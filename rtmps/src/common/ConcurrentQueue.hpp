#ifndef CONCURRENTQUEUE_HPP_
#define CONCURRENTQUEUE_HPP_

#include <boost/thread/mutex.hpp>

namespace common {

template<typename Data>
class concurrent_queue {
 private:
  std::queue<Data> the_queue;
  mutable boost::mutex the_mutex;
 public:
  void push(const Data& data) {
    boost::mutex::scoped_lock lock(the_mutex);
    the_queue.push(data);
  }

  bool empty() const {
    boost::mutex::scoped_lock lock(the_mutex);
    return the_queue.empty();
  }

  Data& front() {
    boost::mutex::scoped_lock lock(the_mutex);
    return the_queue.front();
  }

  Data const& front() const {
    boost::mutex::scoped_lock lock(the_mutex);
    return the_queue.front();
  }

  void pop() {
    boost::mutex::scoped_lock lock(the_mutex);
    the_queue.pop();
  }

  size_t size() {
    boost::mutex::scoped_lock lock(the_mutex);
    return the_queue.size();
  }
};

}  // namespace common
#endif /* CONCURRENTQUEUE_HPP_ */
