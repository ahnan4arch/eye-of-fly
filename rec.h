#ifndef _REC_INC_
#define _REC_INC_

#include <memory>
#include <utility>


namespace vid {
  class IFrameReceiver;
  class StubFrameReceiver;
  
  class FrameReceiverFactory;
}

#include "streamer.h"

class vid::IFrameReceiver {
public:
  typedef std::shared_ptr<IFrameReceiver> pointer;
  
  virtual const Frame get_frame() = 0;
  virtual void set_frame(StreamerSPtr) = 0;
};

class vid::StubFrameReceiver : public IFrameReceiver {
public:

  virtual const Frame get_frame();
  virtual void set_frame(StreamerSPtr);
private:
  
  Frame frame;
};

class vid::FrameReceiverFactory {
public:
  enum class Type {
	Stub,  ///< will always return empty frame
	Test,  ///< will return one (last) frame to all requesting clients
	Real   ///< will return new frame (if any) to all requesing clients
  };

  static IFrameReceiver::pointer make(const Type &t);
};

#include "streamer.h"

#endif
