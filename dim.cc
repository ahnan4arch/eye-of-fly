#include "dim.h"

namespace {

  const size_t sizes[static_cast<unsigned>(vid::ImageType::IMAGE_TYPES_NUM)] = {
	vid::Dim640x480c::size(),
	vid::Dim640x480g::size(),

	vid::Dim320x240c::size(),
	vid::Dim320x240g::size()
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

ImageType
vid::itype(const DecImgParams &par)
{
  struct DemensionRec {
	unsigned w;
	unsigned h;
	// bool   color;
	
	ImageType type;
  };


  // Note: demension fields and color not used now,
  // but this behaviour will be changed in future 
  static const DemensionRec lookup_tlb[] {
	{320, 240, /* bool,  */ ImageType::SIZE320x240c },
    {320, 240, /* false, */ ImageType::SIZE320x240g },
	{640, 480, /* bool,  */ ImageType::SIZE640x480c },
	{640, 480, /* false, */ ImageType::SIZE640x480g }
  };

  switch(par.width / 320) {
  case 1:
	if (par.height == 240) {
	  return (par.out_colors == static_cast<unsigned>(BytesPerPixel::COLORFULL)) ? 
		lookup_tlb[0].type : lookup_tlb[1].type;
	} else {
	  return ImageType::Undefined;
	}
	break;
  case 2:
	if (par.height == 480) {
	  return (par.out_colors == static_cast<unsigned>(BytesPerPixel::COLORFULL)) ? 
		lookup_tlb[2].type : lookup_tlb[3].type;
	} else {
	  return ImageType::Undefined;
	}
	break;
  default:
	return ImageType::Undefined;
  };
}
