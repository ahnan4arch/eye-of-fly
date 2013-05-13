#include "rec.h"

using namespace std;
using namespace vid;

const Frame 
StubFrameReceiver::get_frame()
{
  return frame;
}

void 
StubFrameReceiver::set_frame(StreamerSPtr sp)
{
  //frame = sp->pop_frame();

  FrameWithFlag fp = sp->pop_frame();
  frame = fp.first ? fp.second : frame;
}

IFrameReceiver::pointer
FrameReceiverFactory::make(const Type &t)
{
  switch(t) {
  case Type::Stub:
	// TODO: separate stub and working implmentation
	// return make_shared<StubFrameReciever>();
	// break;
  case Type::Test:
	return IFrameReceiver::pointer(new StubFrameReceiver);
	break;
  case Type::Real:
	throw exception::not_implemented("real reciever not implemented");
	break;
  default:
	break;
  };

  return IFrameReceiver::pointer();
}

