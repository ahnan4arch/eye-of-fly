///
/// @file Memory allocation pools
///
/// @author Alex Karev
///
#ifndef _FRAME_POOL_INC_
#define _FRAME_POOL_INC_

#include <atomic>
#include <memory>
#include <utility>

#include "Poco/MemoryPool.h"

#include "dim.h"
#include "exception.h"

namespace vid {
  class WorkingPool;
  class FramePool;

  typedef std::shared_ptr<Poco::MemoryPool> MemPoolSPtr;
  typedef std::shared_ptr<WorkingPool>      WrkPoolSptr;

  bool operator<(const WrkPoolSptr &lh, const WrkPoolSptr &rh);
}

using Poco::MemoryPool;

///
/// @class WorkingPool
///
/// @brief Record for 'working buffers' table, used to allocate buffers for recieved compressed 
///        image.
///
class vid::WorkingPool {
public:
  WorkingPool(const unsigned pg_num, 
			  const unsigned pg_size=DEFAULT_PAGE_SIZE, 
			  const unsigned allocate=MAX_CAMS_CONFIG * (STREAM_ZBUFFER_SIZE + 1)) :
	pg_num(pg_num),
	requests(0),
	used(0),
	allocated(allocate),
	pool(std::make_shared<MemoryPool>(pg_num * pg_size, allocate, allocate) )
  {}

  friend bool operator<(const WrkPoolSptr &lh, const WrkPoolSptr &rh);
public:

  void *alloc();
  void release(void *p);

  const unsigned pg_number() const { return pg_num; }
private:
  const unsigned pg_num;

  // Statistics
  ///< @todo make allocation more robust based on stat
  std::atomic<unsigned long long> requests;
  std::atomic<unsigned long     > used;
  std::atomic<unsigned long     > allocated;

  MemPoolSPtr pool;
};


///
/// @class FramePool
/// @brief Main, application wide frame allocation pool
///
class vid::FramePool {

  typedef std::vector<WrkPoolSptr> WorkingPoolsVec;
  std::vector<MemPoolSPtr> predefined;

  MemPoolSPtr bootstrap;
  WorkingPoolsVec wrk_pools;
public:
  typedef std::shared_ptr<FramePool> FramePoolSPtr;

  typedef std::pair<unsigned, void*> WorkingSlab;
  
  /// most field (if not mentioned) are in bytes
  struct Stat {
	std::atomic<long long> allocated;             
	std::atomic<long long> total_preallocated;
  };

public:

  WorkingSlab alloc_wrk(size_t size);
  void release_wrk(const WorkingSlab &wrk);
   
  void    *alloc_predef(ImageType type);
  void   release_predef(ImageType type, void *ptr);
  
  const size_t bootstrap_size() const; ///< predefined bootstrap buffer size (almost always = system page size)

  void    *alloc_bootstrap();
  void   release_bootstrap(void *);

  static FramePoolSPtr global_pool() {
	return (self) ? self : (self.reset( new FramePool ), self);
  }

  unsigned long wrk_pool_idx(const size_t size) const;

private:

  FramePool();

  FramePool(const FramePool &other) = delete;
  FramePool &operator=(const FramePool &other) = delete;

private:

  void init_page_size();
  unsigned long pages_num(const size_t size) const;

private:

  static std::shared_ptr<FramePool> self;

  long sys_page_size; ///< System page size. Should be const
};


inline void *
vid::FramePool::alloc_bootstrap() 
{
  return bootstrap->get();
}

inline void
vid::FramePool::release_bootstrap(void *ptr) 
{
  bootstrap->release(ptr);
}


#endif
