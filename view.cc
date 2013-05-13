#include <algorithm>
#include <chrono>
#include <functional>
#include <iterator>
#include <sstream>
#include <memory>
#include <cstdlib>

#include <FL/glu.h>
#include <FL/glut.H>

//#include "disp.h"
#include "config.h"
#include "exception.h"
#include "stat.h"
#include "util.h"
#include "view.h"

using namespace std;
using namespace vid;

namespace {
  class ViewGenerator;
}

namespace { 
  const GLfloat col_fwhite[] = {1.f,1.f,1.f,1.f};
  const GLfloat col_fblack[] = {0.f,0.f,0.f,1.f};
}

std::ostream &vid::operator<<(std::ostream &o, const IView &iv)
{
  static const char sep = ',';
  o << "g(" << iv.x() << sep << iv.y() << sep << iv.w() << sep << iv.h() <<  ')';

  return o;
}

void
CamViewStub::draw()
{
  //glColor4i(100, 100, 100,1);
  glBegin(GL_QUADS);
  glColor3f(0.15,0.15,0.15);
  glVertex2i(     x() + 1,     y() + 1 );
  glVertex2i( w()+x() - 1,     y() + 1 );
  glVertex2i( w()+x() - 1, h()+y() - 1 );
  glVertex2i(     x() + 1, h()+y() - 1 );
  glEnd();

  //  gl_color(FL_YELLOW);
  //gl_font( FL_HELVETICA, 10);
  //gl_draw( "No stream", (int) x()+15, (int) y()+15);

  //dbg( "Drawing rec (" << x() << LSP << y() << LSP << w() << LSP << h()  << ')');
}


void 
CamViewTest::draw_tex()
{
  using namespace std;


  FrameRawSPtr fp;
  ImageType type;

  tie(fp, ignore, type) = receiver->get_frame();

  if (fp) {

	glBindTexture( GL_TEXTURE_2D, tex);

#if 1
#warning "Test version being compiled..."
	if (last_type != type) {
	  img_geom = get_geom(type);	  
	  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  
	  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 
					img_geom.width, img_geom.height, 
					0,  // border, unused
					GL_RGB, GL_UNSIGNED_BYTE, fp.get() );	   
	  
	  last_type = type;
	} else {
	  glTexSubImage2D( GL_TEXTURE_2D, 0,
					0, 0, // x,y offsets
					img_geom.width, img_geom.height, 
					GL_RGB, GL_UNSIGNED_BYTE, fp.get() );	   
	  
	}
#else
	img_geom = get_geom(type);	  
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 
				  img_geom.width, img_geom.height, 
				  0,  // border, unused
				  GL_RGB, GL_UNSIGNED_BYTE, fp.get() );	   
#endif

	glEnable(GL_TEXTURE_2D);
	glColor3fv(col_fwhite);
	glInterleavedArrays( GL_T2F_V3F, 0, view.get() );
	glDrawArrays( GL_QUADS, 0, 4 );  
	glDisable(GL_TEXTURE_2D);
  }
}

void
CamViewTest::draw()
{
  draw_tex();
}

void 
CamViewTest::resize(const Geometry &g )
{
  IView::resize(g);
  update_vertexes();
}

void
CamViewTest::update_vertexes()
{
  // Should be
  // const GLfloat view[][COORD_NUM] = {
  // 	{ 0, 0, x()      , y()       },
  // 	{ 1, 0, x() + w(), y()       },
  // 	{ 1, 1, x() + w(), y() + h() },
  // 	{ 0, 1, x()      , y() + h() }
  //  };
  
  VVAL(0, VCoord::VRT_X) = x() + 1;
  VVAL(0, VCoord::VRT_Y) = y() + 1;
  VVAL(0, VCoord::VRT_Z) = 0.f;

  VVAL(1, VCoord::VRT_X) = x() + w() - 1;
  VVAL(1, VCoord::VRT_Y) = y() + 1;
  VVAL(1, VCoord::VRT_Z) = 0.f;

  VVAL(2, VCoord::VRT_X) = x() + w() - 1;
  VVAL(2, VCoord::VRT_Y) = y() + h() - 1;
  VVAL(2, VCoord::VRT_Z) = 0.f;

  VVAL(3, VCoord::VRT_X) = x() + 1;
  VVAL(3, VCoord::VRT_Y) = y() + h() - 1;
  VVAL(3, VCoord::VRT_Z) = 0.f;
}

void
CamViewTest::init()
{
  // note: can throw!
  view.reset(new GLfloat[VERTEX_NUM * 
						 static_cast<unsigned>(VCoord::COORD_NUM)], 
			 std::default_delete<GLfloat[]>() );

  VVAL(0, VCoord::TEX_S ) = 0;
  VVAL(0, VCoord::TEX_T ) = 1;

  VVAL(1, VCoord::TEX_S ) = 1;
  VVAL(1, VCoord::TEX_T ) = 1;

  VVAL(2, VCoord::TEX_S ) = 1;
  VVAL(2, VCoord::TEX_T ) = 0;

  VVAL(3, VCoord::TEX_S ) = 0;
  VVAL(3, VCoord::TEX_T ) = 0;

  update_vertexes();
 
  glGenTextures(1, &tex);
  glEnable(GL_TEXTURE_2D);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

  last_type = ImageType::Undefined;
}


class ViewGenerator {
public:

  ViewGenerator(unsigned c, unsigned r, 
				unsigned w, unsigned h, 
				unsigned left_margins, unsigned top_margins) : 
	cols(c), rows(r), 
	W(w), H(h),
	width(w/c), height(h/r), 
	current(0), max(c*r), lm(left_margins), tm(top_margins)  {}

  IView::pointer operator()() {

	if (current >= max) // start  over
	  current = 0;

	// yes, i know that braces are not really needed
	unsigned x_offset = lm + (current % cols) * width;
	unsigned y_offset = tm + H - (current / rows + 1) * height;

	// unsigned x_offset = lm + (current % cols) * width;
	// unsigned y_offset = tm + (current / rows) * height;

	current++;

	// dbg("gen: num " << current << " (x,y) => (" << x_offset << LSP << y_offset  << ')' );

	return IView::pointer( new CamViewStub(x_offset, y_offset, width, height) );
  }

private:
  unsigned cols, rows, W, H,width, height, current, max, lm, tm;
};

MainView::MainView(int W, int H,
				   unsigned c, unsigned r, 
				   const char *l,
				   unsigned left_margins,  unsigned right_margins,
				   unsigned top_margins,   unsigned bottom_margins ):
  Fl_Gl_Window(W + left_margins + right_margins, H + top_margins + bottom_margins, l),
  c(c), r(r),
  left_marg(left_margins),
  right_marg(right_margins),
  top_marg(top_margins),
  bottom_marg(bottom_margins)
{
  using namespace std;
  
  register unsigned n = c * r;
  cviews.reserve(n);
  generate_n(back_inserter(cviews), n, ::ViewGenerator(c, r, W, H, left_margins, top_margins) );

  stat.reset( new StatPanel(0, 0, W + left_margins + right_margins, DEFAULT_STAT_HEIGHT));
}


inline 
unsigned
MainView::rows() const
{
  return this->r;
}

inline 
void
MainView::rows(unsigned rows)
{
  this->r = rows;
}

inline
unsigned 
MainView::cols() const
{
  return this->c;
}

inline
void     
MainView::cols(unsigned cols)
{
  this->c = cols;
}

inline 
void 
//MainView::set_cell(unsigned c, unsigned r, IView::pointer sp)
MainView::set_test_cell(unsigned i, unsigned j, IFrameReceiverSPtr rec)
{
  if (i > cols()-1 || j > rows()-1)
	throw exception::out_of_range_error("rows and (or) columns indexes out of view talbe bounds");

  ViewPlacerEx placer( i, j, cols(), rows(), 
					   w()-left_marg-right_marg, 
					   h()-top_marg-bottom_marg, 
					   left_marg, bottom_marg );
  cviews[j * cols() + i] = IView::pointer(new CamViewTest( placer(), rec));
}


int
MainView::run(int argc, char *argv[])
{
  MainView *w = new MainView(MW_WIDTH  + (MW_OFFSET<<1),
							 MW_HEIGHT + (MW_OFFSET<<1),
							 MW_COLS, MW_ROWS,
							 "IO & GUI test");

  w->show(argc, argv);
  return Fl::run();
}


int
MainView::run_test( IFrameReceiver::pointer rec, StreamerSPtr stream, int argc, char **argv)
{
  MainView *w = new MainView(MW_WIDTH  + (MW_OFFSET<<1),
  							 MW_HEIGHT + (MW_OFFSET<<1),
  							 MW_COLS, MW_ROWS,
  							 "IO & GUI test",
							 MW_OFFSET, MW_OFFSET,
							 MW_OFFSET, DEFAULT_STAT_HEIGHT);

  for(int i=0; i < MW_ROWS;++i) {
	for(int j=0; j < MW_COLS; ++j) {
	  w->set_test_cell(i,j, rec);
	}
  }
 
  w->show(argc, argv);
  
  while(Fl::check() > 0) {
  	rec->set_frame(stream);
	//dbg("Redraw event...");
  	w->redraw();
  }

  return EXIT_SUCCESS;
}

void
MainView::draw()
{
  using namespace std;

  if (!valid()) {
	reinit_screen();
	reinit_views();
  }

  // dbg("Start draw...");



  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  stat->draw();

  auto t1 = chrono::high_resolution_clock::now();
  for_each( std::begin(cviews), std::end(cviews), mem_fn(&IView::draw) );
  auto t2 = chrono::high_resolution_clock::now();
  auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

  StreamStat::instance()->update_gl_redraw_time(dt);

  // dbg("Draw done!");
}

void
MainView::reinit_screen()
{
  glEnable(GL_DEPTH_TEST);	
  // glDisable(GL_BLEND);
  glClearColor(0.15f, 0.15f, 0.15f, 1.f);
  glEnable( GL_TEXTURE_2D );
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, w(), 0, h());

  gl_color(FL_YELLOW);
  gl_font(FL_HELVETICA, 12);
}

void
MainView::reinit_views()
{
  unsigned u = 0;

  for_each( std::begin(cviews), std::end(cviews), [this, &u] (const IView::pointer iv) {
	  unsigned i = u % this->c;
	  unsigned j = u / this->r;

	  iv->resize( ViewPlacerEx(i, j,
							   this->cols(), this->rows(),
							   this->w()-left_marg-right_marg, 
							   this->h()-top_marg-bottom_marg,
							   left_marg, bottom_marg) );

	  // dbg("Upd geom ["<< u <<"] => " << *iv);

	  u++; } );
}

void
IView::resize(const Geometry &g)
{
  x(g.x);
  y(g.y);
  w(g.w);
  h(g.h);
}


void
StatPanel::draw()
{
  // glDisable(GL_BLEND);
  StreamStat::pointer sp = StreamStat::instance();

  std::ostringstream os;

  //glColor3f(1.f,1.f,1.f);

  //  os.clear();
  //os.str("");
  double draw_us = sp->avg_gl();
  os << "Screen redraw @ "
  	 << std::setprecision(2) << std::fixed 
  	 << (draw_us / 1000.)
  	 << " ms (whole view), ~ " 
	 << (1000000. / draw_us)
	 << " fps"
	 << std::endl ;
	
  gl_draw(os.str().c_str(), static_cast<int>(x() + 10U), static_cast<int>(y() + 50U));

  os.clear();
  os.str("");

  os << "JPEG decompression @ " 
	 << std::setprecision(2) << std::fixed 
	 << (sp->avg_decode() /1000.)
	 << " ms (per frame)" 
	 << std::endl;

  gl_draw(os.str().c_str(), static_cast<int>(x() + 10U), static_cast<int>(y() + 30U));

  os.clear();
  os.str("");
  os << "Frame retrieval @ "
	 << std::setprecision(2) << std::fixed 
	 << (sp->avg_net() / 1000.)
	 << " ms" 
	 << std::endl ;
	
  gl_draw(os.str().c_str(), static_cast<int>(x() + 10U), static_cast<int>(y() + 10U));

  //  glEnable(GL_BLEND);
}
