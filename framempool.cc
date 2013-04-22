#include "config.h"
#include "framempool.h"

using Poco::MemoryPool;
using namespace vid;

#define CAST2UINT(v) static_cast<unsigned>(v)

FramePool::FramePool() 
{
  register unsigned sz = static_cast<unsigned>(ImageType::IMAGE_TYPES_NUM);
  predefined.reserve(sz);

  for(unsigned i = 0; i < sz; ++i)  {
	predefined.push_back(std::make_shared<MemoryPool>( vid::get_size(i),  
													   STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG * vid::get_size(i),
													   STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG * vid::get_size(i)));
  }

  bootstrap = std::make_shared<MemoryPool>(SYS_PAGE_SIZE);
}

const size_t
FramePool::boostrap_size()  const
{
  return bootstrap->blockSize();
}

void *
FramePool::alloc_predef(ImageType type)
{
  return predefined[CAST2UINT(type)]->get();
}

void   
FramePool::release_predef(ImageType type, void *ptr)
{
  predefined[CAST2UINT(type)]->release(ptr);
}
