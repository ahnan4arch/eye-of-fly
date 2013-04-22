///
/// @file Memory allocation pools
///
/// @author Alex Karev
///
#ifndef _FRAME_POOL_INC_
#define _FRAME_POOL_INC_

#include <atomic>
#include <memory>

#include "Poco/MemoryPool.h"

#include "dim.h"
#include "exception.h"

namespace vid {
  class FramePool;
}

using Poco::MemoryPool;

///
/// @class FramePool
/// @brief Main, application wide frame allocation pool
///
class vid::FramePool {


  typedef std::shared_ptr<Poco::MemoryPool> MemPoolSPtr;

  std::vector<MemPoolSPtr> predefined;

  MemPoolSPtr bootstrap;
public:
  typedef std::shared_ptr<FramePool> FramePoolSPtr;
  
  /// most field (if not mentioned) are in bytes
  struct Stat {
	std::atomic<long long> allocated;             
	std::atomic<long long> total_preallocated;
  };

public:
 
  void    *alloc_predef(ImageType type);
  void   release_predef(ImageType type, void *ptr);
  
  const size_t bootstrap_size() const; ///< predefined bootstrap buffer size (almost always = system page size)

  void    *alloc_bootstrap();
  void   release_bootstrap(void *);

  static FramePoolSPtr global_pool() {
	return (self) ? self : (self.reset( new FramePool), self);
  }

  ///< @todo alloc and release custom frames
private:

  FramePool();

  FramePool(const FramePool &other) = delete;
  FramePool &operator=(const FramePool &other) = delete;

  static std::shared_ptr<FramePool> self;
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
