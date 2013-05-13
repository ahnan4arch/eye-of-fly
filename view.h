#ifndef _FLY_INC_
#define _FLY_INC_

#include <iostream>
#include <memory>

#include <FL/gl.h>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>


#define MW_OFFSET 2
//#define MW_WIDTH  640
//#define MW_HEIGHT 480
#define MW_WIDTH  980
#define MW_HEIGHT 640

#define MW_COLS 4
#define MW_ROWS 4

#define DEFAULT_STAT_HEIGHT 80

namespace vid 
{
  class IView;
  class CamViewStub;
  class CamViewTest;

  class MainView;
  class IFrameReceiver;
  typedef std::shared_ptr<IFrameReceiver> IFrameReceiverSPtr;

  struct Geometry;
  class ViewPlacer; ///< @todo remove
  class ViewPlacerEx;

  std::ostream &operator<<(std::ostream &, const IView &);

  class StatPanel;
}

struct vid::Geometry {
  unsigned x, y, w, h;
};

class vid::ViewPlacer {
public:
  ViewPlacer(unsigned c, unsigned r, 
			 unsigned w, unsigned h, 
			 unsigned left_margins, unsigned top_margins) : 
	cols(c), rows(r), 
	W(w),
	width(w/c), height(h/r), current(0), max(c*r), lm(left_margins), tm(top_margins)  {}

public:
  unsigned x() const { return lm     + (current % cols     ) * width  ; }
  unsigned y() const { return W - tm - (current / rows + 1 ) * height ; }

  unsigned w() const { return width;  }
  unsigned h() const { return height; }
private:
  unsigned cols, rows, W, width, height, current, max, lm, tm;
};

class vid::ViewPlacerEx {
public:
  ViewPlacerEx(unsigned i, unsigned j, 
			 unsigned c, unsigned r, 
			 unsigned W, unsigned H, 
			 unsigned left_margins, unsigned top_margins) : 
	i(i), j(j),
	cols(c), rows(r), 
	H(H),
	width(W/c), height(H/r), max(c*r), lm(left_margins), tm(top_margins)  {}

public:
  unsigned x() const { return lm     + ( pos() % cols     ) * width  ; }
  unsigned y() const { return tm + H - ( pos() / rows + 1 ) * height ; }

  unsigned w() const { return width;  }
  unsigned h() const { return height; }

  const Geometry operator()() const { return { x(), y(), w(), h() }; }

private:
  unsigned pos() const { return j * cols + i; }
private:
  unsigned i, j, cols, rows, H, width, height, max, lm, tm;
};


class vid::IView {
public:
  typedef std::shared_ptr<IView> pointer;

  IView(unsigned x, unsigned y, 
		unsigned w, unsigned h) : 
	xm(x), ym(y), 
	wm(w), hm(h) {}

public:

  virtual void draw()   = 0;

public:
  
  void     x(unsigned x)        { this->xm  = x; }
  unsigned x(          ) const  { return     xm; }

  void     y(unsigned y)        { this->ym = ym; }
  unsigned y(          ) const  { return     ym; }

  void     h(unsigned h)        { this->hm = hm; }
  unsigned h(          ) const  { return     hm; }

  void     w(unsigned w)        { this->wm = wm; }
  unsigned w(          ) const  { return     wm; }

  virtual void resize(const ViewPlacerEx &vp)  { resize(vp()); }
  virtual void resize(const Geometry &);
private:
  unsigned xm;
  unsigned ym;
  unsigned wm;
  unsigned hm;
};


class vid::StatPanel : public vid::IView {
public:

  typedef std::shared_ptr<StatPanel> pointer;

  StatPanel(unsigned x, unsigned y, 
			unsigned w, unsigned h) : IView(x,y,w,h) {}

public:
  virtual void draw();
};


#include "rec.h"


class vid::CamViewStub : public vid::IView {
public:
  CamViewStub(unsigned x, unsigned y, 
			  unsigned w, unsigned h) : IView(x,y,w,h) {}

  virtual void draw();

};


class vid::CamViewTest : public vid::IView {
  enum class VCoord {
	TEX_S,
	TEX_T,
	VRT_X,
	VRT_Y,
	VRT_Z,
	COORD_NUM
  };

#define VERTEX_NUM 4
#define VVAL(i,f) view.get()[i * static_cast<int>(VCoord::COORD_NUM) + static_cast<int>(f)] 

public:
  CamViewTest(const Geometry &g,
			  IFrameReceiverSPtr receiver=IFrameReceiverSPtr()) : 
	IView(g.x, g.y, g.w, g.h), 
	tex(0), 
	receiver(receiver)
  {
	init();
  }

  CamViewTest(unsigned x, unsigned y, 
			  unsigned w, unsigned h, 
			  IFrameReceiverSPtr receiver=IFrameReceiverSPtr()) : 
	IView(x, y, w, h),
	tex(0),
	receiver(receiver) 
  {	
	init();
  }

  ~CamViewTest()
  {
	glDeleteTextures(1, &tex);
  }

public:
  virtual void draw();
  virtual void resize(const Geometry &);

protected:
  void draw_tex();
  void update_vertexes();
private:
  void init();

private:
  GLuint tex;
  std::shared_ptr<GLfloat> view;
  DecImgParams img_geom;
  ImageType last_type;
protected:
  IFrameReceiverSPtr receiver;
};


class vid::MainView : public Fl_Gl_Window {
public:
  MainView(int W, int H,
		   unsigned cols, unsigned rows, 
		   const char *l=0,
		   unsigned left_margins=MW_OFFSET, unsigned  right_margins=MW_OFFSET,
		   unsigned  top_margins=MW_OFFSET, unsigned bottom_margins=MW_OFFSET );
public:
  void draw();

  static int run(int, char **);
  static int run_test( IFrameReceiver::pointer rec, StreamerSPtr stream, int, char **);

  unsigned rows(        ) const;
  void     rows(unsigned)      ;

  unsigned cols(        ) const;
  void     cols(unsigned)      ;

  //  void set_cell(unsigned, unsigned, IView::pointer);
  void set_test_cell(unsigned, unsigned, IFrameReceiverSPtr);

private:
  void reinit_screen();
  void reinit_views();

private:
  unsigned c, r;
  std::vector<IView::pointer> cviews;

  StatPanel::pointer stat;

  unsigned left_marg;
  unsigned right_marg;
  unsigned top_marg;
  unsigned bottom_marg;
};

#endif
