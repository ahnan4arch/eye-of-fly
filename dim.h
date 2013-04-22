///
/// @file Predefined images constants and enumerations
///
/// @author Alex Karev
///
#ifndef _DIMS_INC_
#define _DIMS_INC_

#include <utility>
#include <cstdlib>

namespace vid {

  enum BytesPerPixel {
	GRAYSCALE = 1,
	COLORFULL = 3
  };

  enum ColorOrder {
	RGB,
	BGR,
	G
  };

  /// @note In case you udate this enum don't forget to add to get_size table new field
  enum class ImageType {
	// 640x480
	SIZE640x480c, // color
	SIZE640x480g, // grayscale
	
	// 320x240
	SIZE320x240c,
	SIZE320x240g,

	IMAGE_TYPES_NUM
  };

  enum class Fps {
	FPS3 = 3,
	FPS7 = 7,
	FPS10 = 10,
	FPS15 = 15,
	FPS20 = 20,
	FPS25 = 25,
	FPS30 = 30,
	FPS60 = 60,
	FPS_NUM
  };
  
  template<size_t W, size_t H, BytesPerPixel S, ColorOrder C=ColorOrder::G> struct Dim;

  /// Help functions
  const size_t   get_size(ImageType type);
  const size_t   get_size(unsigned type);

  unsigned       get_fps(Fps fps);
}



///
/// @struct Dim
///
/// @brief Conventional struct to get some constant image parameters
///
/// @param W width
/// @param H height
/// @param S bytes per pixel
///
template<size_t W, size_t H, vid::BytesPerPixel S, vid::ColorOrder C>
struct vid::Dim {

  static constexpr size_t size()  { ///< returns pure raw, decompressed image size in bytes
	return W * H * S;
  }

  static size_t height() {
	return H;
  }

  static size_t width() {
	return W;
  }

  static unsigned bytes_per_pixel() {
	return static_cast<unsigned>(S);
  }

};

namespace vid {
  ///< Some predifined common image demensions
  typedef Dim<640,480, vid::BytesPerPixel::COLORFULL> Dim640x480c;
  typedef Dim<640,480, vid::BytesPerPixel::GRAYSCALE> Dim640x480g;

  typedef Dim<320,240, vid::BytesPerPixel::COLORFULL> Dim320x240c;
  typedef Dim<320,240, vid::BytesPerPixel::GRAYSCALE> Dim320x240g;

  enum VideoModeFields {
	ImageTypeField,
	FpsField
  };

  typedef std::tuple<ImageType, Fps> VideoMode;
}


#endif
