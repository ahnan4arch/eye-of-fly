#include "dim.h"

namespace {

  const size_t sizes[static_cast<unsigned>(vid::ImageType::IMAGE_TYPES_NUM)] = {
	vid::Dim640x480c::size(),
	vid::Dim640x480g::size(),

	vid::Dim320x240c::size(),
	vid::Dim320x240g::size(),
  };

}


using namespace vid;

const size_t vid::get_size(ImageType type) {
  return ::sizes[static_cast<unsigned>(type)];
}

const size_t vid::get_size(unsigned type) {
  return ::sizes[static_cast<unsigned>(type)];
}


unsigned vid::get_fps(Fps fps) {
  return static_cast<unsigned>(fps);
}
