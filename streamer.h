///
/// @file JESStreamer streamer for data recieved from data
///
/// @author Alex Karev
///
#ifndef _STREAMER_INC_
#define _STREAMER_INC_

#include <atomic>
#include <chrono>
#include <memory>
#include <tuple>
#include <thread>
#include <type_traits>

#include <cassert>
#include <cstdint>

#include <endian.h>

#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "config.h"
//#include "cam.h"
#include "framempool.h"
#include "dim.h"
#include "jpegd.h"


#define SCAST2BYTE_PTR(bp) static_cast<uint8_t*>(bp)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__       
// #define CONV_FROM_BE16(v) __builtin_bswap16(v)
// #define CONV_FROM_BE32(v) __builtin_bswap32(v)
#define CONV_FROM_BE16(v) htobe16(v)
#define CONV_FROM_BE32(v) htobe32(v)
#else 
#define CONV_FROM_BE16(v) v
#define CONV_FROM_BE32(v) v
#endif


// Note: forward declarations, inlcudes may follow
// TODO: rewrite it
namespace vid {

  enum class FrameField : unsigned {
	ImageData,
	Size,
	ImageType
  };

  typedef std::shared_ptr<uint8_t> FrameRawSPtr;

  typedef std::tuple<FrameRawSPtr,   ///< Processed frame in memory
					 size_t,         ///< Size of processed frame
					 ImageType
					 > Frame;

  typedef std::pair<bool, Frame> FrameWithFlag;

  class FrameDeleter;


  enum class WBField : unsigned {
	SlabInfo,
	Offset,  
    Size
  };

  typedef std::tuple<FramePool::WorkingSlab,    ///< pointer to start is a second elementh
					 size_t,         ///< offset, where to put data
					 size_t          ///< total incoming jpeg size (+jes header)
					 > WrkBuffer;

  class Streamer;
  class JESStreamer;
  struct StreamerStat;

  typedef std::shared_ptr<Streamer> StreamerSPtr;
}


#include "cam.h"


namespace vid {
  namespace ba = boost::asio;
  
  struct DisposeFrame {
	FramePool &fp;
	const ImageType type;

	DisposeFrame(FramePool &f, ImageType t) : fp(f), type(t) {}

	void operator()(uint8_t *to_delete) {
	  fp.release_predef(type, to_delete);
	}
  };



  enum class BBufferField : unsigned {
	RawData,
	JESHeader,
	BytesWritten,
    Size,
  };

  typedef std::tuple<std::uint8_t *, ///< Incoming packet data of JPEG image
					 std::uint8_t *, ///< Active Jes header pointer
					 int,            ///< Offset, where to write next byte
					 const size_t
					 > BootstrapBuffer;

  // 
  // Note: gcc <= 4.6 don't support 'underlying_type'
  // 
  // template<class Enum> 
  // constexpr 
  // typename std::underlying_type<en>::type
  // field(Enum en) {
  // 	return static_cast<typename std::underlying_type<en>::type>(en);
  // }

  template<class Enum> 
  constexpr 
  unsigned
  field(Enum en) {
  	return static_cast<unsigned>(en);
  }


  class BinaryParser;
  struct skip;

  template<class T> void cast2val(uint8_t *ptr, T &v);
  BinaryParser &operator>>( BinaryParser &lh, const skip &rh);
}



class vid::FrameDeleter {
  FramePool &pool;
  ImageType type;
public:
  FrameDeleter(FramePool &pool, ImageType type) : pool(pool),
												  type(type) {}
  void operator()(std::uint8_t *p) {
	pool.release_predef(type, p);
  }
};



class vid::BinaryParser {

  uint8_t *buffer, *where;
  size_t len;

  friend struct skip;
  friend      BinaryParser &operator>>(vid::BinaryParser &lh, const vid::skip &rh);


public:

  BinaryParser(uint8_t *buffer, size_t len) : 
	buffer(buffer), 
	where(buffer), 
	len(len)  
  {}

  BinaryParser &operator>>(uint8_t  &v);
  BinaryParser &operator>>(uint16_t &v);
  BinaryParser &operator>>(uint32_t &v);
  
  inline BinaryParser &skip(const size_t &skip);

  typedef uint8_t   byte_placeholder;
  typedef uint16_t short_placeholder;
  typedef uint32_t  word_placeholder;
  
  uint8_t *offset() const {
	return where;
  }

  uint8_t *data() const {
	return buffer;
  }

};


#include <iostream>
#include <iomanip>

template<class T>
void vid::cast2val(uint8_t *ptr, T &v) {
  v = *reinterpret_cast<T *>(ptr);

  // using namespace std;
  // std::cerr << "byte 0 " << hex << (int) *ptr << endl;
  // std::cerr << "byte 1 " << hex << (int)*(ptr + 1) << endl;
  // std::cerr << "byte 2 " << hex << (int)*(ptr + 2) << endl;
  // std::cerr << "byte 3 " << hex << (int)*(ptr + 3) << endl;

  // std::cerr << "Value "  << (int)  *reinterpret_cast<T *>(ptr) << std::endl;
}

inline
vid::BinaryParser &
vid::BinaryParser::operator>>(uint8_t  &v)
{
  assert(len - (where - buffer) >= sizeof(uint8_t)); 

  cast2val(where, v);
  where += sizeof(uint8_t);

  return const_cast<BinaryParser&>(*this);
}

inline
vid::BinaryParser &
vid::BinaryParser::operator>>(uint16_t  &v)
{
  assert(len - (where - buffer) >= sizeof(uint16_t)); 

  cast2val(where, v);
  v = CONV_FROM_BE16(v);

  //  v = cast2val(buffer);
  //  v = *rintrepret_cast<uint16_t *>(buffer);
  where += sizeof(uint16_t);

  return const_cast<BinaryParser&>(*this);
}


inline
vid::BinaryParser &
vid::BinaryParser::operator>>(uint32_t  &v)
{
  assert(len - (where - buffer) >= sizeof(uint32_t)); 

  cast2val(where, v);
  v = CONV_FROM_BE32(v);

  where += sizeof(uint32_t);

  return const_cast<BinaryParser&>(*this);
}


struct vid::skip {
  size_t to_skip;
  explicit skip(const size_t to_skip) : to_skip(to_skip) {}
};



///
/// @todo Implement state machine hierarchy
///
struct State {
  virtual void doit() = 0;
};

struct InitState : State {
  virtual void doit() {
  }
};


/// @todo add some useful implementation
struct vid::StreamerStat {
  std::atomic<long long> counter;
};

class vid::Streamer {
public:

  virtual void start_reading() = 0;
  virtual void start_reading( const ba::ip::tcp::socket::native_handle_type &so ) = 0;

  virtual void       set_camera(CameraSPtr) = 0;
  virtual CameraSPtr get_camera()           = 0;

  virtual ba::io_service &get_io_service() = 0;

  virtual void close() = 0;

  //  virtual void bint_to_stream(ba::io_service &io) = 0;
  
  typedef std::shared_ptr<void> ISockSPtr;
  virtual void store_sock_holder(ISockSPtr) = 0;

  // virtual const Frame pop_frame()   = 0;
  // virtual size_t zqueue_len() const = 0;

  virtual const FrameWithFlag   pop_frame()       = 0;
  virtual                       size_t zqueue_len() const = 0;


  virtual ~Streamer() {}
};

///
/// @class jes_streamer 
///
/// @brief get and parse data stream from camera
///
class vid::JESStreamer : 
  Streamer, 
  public std::enable_shared_from_this<JESStreamer>, 
  boost::noncopyable
{

  typedef std::pair<void *, size_t> ToReadInPtr;

  //protected:
  enum class State : unsigned {
      InitState, ///< really should be called 'interframe state'
		ReadJPEGFirstMarkState,
		ReadJPEGSecondMarkState,
		ReadJESFirstMarkState,
		ReadJESSecondMarkState,
		ReadJESHeaderState,
		GetJESHeaderState,
		ReadJPEGFrameState,
      ErrorState,
	  NoDataState,
  };

  State state;                          ///< current state of streamer
private:

  struct JESHdr {
	std::uint32_t size;

	void clear() {
	  size = 0;
	}
  };

public:

  typedef std::shared_ptr<Streamer> pointer;

  static pointer make_streamer(ba::io_service &io, 
			  FramePool &mpool,
			  const ba::ip::tcp::socket::native_handle_type &so);

  static pointer make_streamer(CameraSPtr, ba::io_service &io, 
			  FramePool &mpool,
			  const ba::ip::tcp::socket::native_handle_type &so);

private:

  ///< @todo rewrite verbose constructors via constructor delegation from C++11 (gcc >= 4.7)
  JESStreamer(ba::io_service &io, 
			  FramePool &mpool,
			  const ba::ip::tcp::socket::native_handle_type &so) : 
	state(State::InitState),
	strand(io), 
	insock(io, ba::ip::tcp::v4() ,so), 
	cancel(false),
	mpool(mpool),
	bootstrap( SCAST2BYTE_PTR(mpool.alloc_bootstrap()), nullptr, 0, mpool.bootstrap_size() ),
	zqueue(STREAM_ZBUFFER_SIZE),
	jdec(std::make_shared<JPEGDecoder>()),
	iparam(std::make_shared<DecImgParams>())
  {
	start_reading();
  }

  JESStreamer(CameraSPtr parent,
			  ba::io_service &io, 
			  FramePool &mpool,
			  const ba::ip::tcp::socket::native_handle_type &so) : 
	state(State::InitState),
	camera(parent),
	strand(io), 
	insock(io, ba::ip::tcp::v4() ,so), 
	cancel(false),
	mpool(mpool),
	bootstrap( SCAST2BYTE_PTR(mpool.alloc_bootstrap()), nullptr, 0, mpool.bootstrap_size() ),
	zqueue(STREAM_ZBUFFER_SIZE),
	jdec(std::make_shared<JPEGDecoder>()),
	iparam(std::make_shared<DecImgParams>())
  {
	start_reading();
  }

public:

  ~JESStreamer() {

	if (std::get<field(WBField::SlabInfo)>(working_buffer).second != nullptr)
	  release_wrk_buff();

	mpool.release_bootstrap(std::get<field(BBufferField::RawData)>(bootstrap));

	///
	/// @todo where we should close socket. It'll be closed also inside connection 
	/// class (in StreamSocket).
	///
	if (!cancel)
	  insock.close();

	std::lock_guard<std::mutex> lock(zqlock);
	zqueue.clear();
  }

public:

  virtual void start_reading();
  virtual void start_reading( const ba::ip::tcp::socket::native_handle_type &so );

  ///< @todo reset buffers
  virtual void stop_reading();

  virtual void       set_camera(CameraSPtr);
  virtual CameraSPtr get_camera();

  virtual ba::io_service &get_io_service() { return insock.get_io_service(); }

  virtual void close();

  virtual void store_sock_holder(ISockSPtr);

  virtual const std::pair<bool,Frame>   pop_frame();
  virtual                       size_t zqueue_len() const;

private:

  void reading_handler_stub(const boost::system::error_code& er, size_t size );

  uint8_t    *bootstrap_wrk_offset();
  State       setInitState();

  uint8_t *const   bootstrap_begin() const;

  uint8_t    *working_buff_offset()  const;
  size_t      left_in_working_buff() const;
  uint8_t *const  working_buff_begin() const;
  size_t      working_buff_size() const;

  size_t      reset_bootstrap();
  size_t      update_bootstrap(uint8_t *p);

  void        set_hdr_pointer(uint8_t *hdr=nullptr);
  uint8_t    *get_hdr_pointer();
  void        parse_jes_hdr();

  size_t      move_to_begin_bootstrap(uint8_t *buffer, const size_t left);

  ToReadInPtr search_jpeg_mark(const size_t size); 
  ToReadInPtr load_jpeg       (const size_t size);
  ToReadInPtr load_jes_hdr    (const size_t size);

  size_t left_in_bootstrap() const;
  void inc_bootstrap_written(const size_t size);
 
  void set_state(State);


  void init_wrk_buff(const size_t size);
  void release_wrk_buff();

private:
  CameraWeakRef camera;

  ba::strand strand;
  ba::ip::tcp::socket insock;


  ISockSPtr pso; 
  ///<  stored here for keeping counter above zero, to not to delete it 
  ///<  occasionally in another parts of code
  
  bool cancel;

  FramePool &mpool;
  BootstrapBuffer bootstrap;            ///< bootstrap buffer for header of camera content. is one page size
  boost::circular_buffer<Frame> zqueue; ///< incoming frame buffer of ready for displaying images
  mutable std::mutex zqlock;

  WrkBuffer working_buffer;            ///< incoming jpeg frame (comressed) buffer

  JESHdr jes_hdr;

  std::shared_ptr<JPEGDecoder>    jdec;
  std::shared_ptr<DecImgParams> iparam; 

private: // benchmark time stamps

  std::chrono::time_point<std::chrono::high_resolution_clock> net_t1;

};

namespace vid {
  //  void operator>>(const BinaryParser &rh, JESStreamer::JESHdr &lh);
}

#endif


