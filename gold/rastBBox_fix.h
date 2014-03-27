/*
 *
 *  Header for a Bbox approach to rasterization
 *
 *
 *
 *
 *
 */


/* f_1 -> given a vector and 
 *        zedd buffer, return some cost
 */

#include "helper.h"
#include "zbuff.h"
#include <math.h>
#include <vector>


static const char kQuadCullBackHits_fix[] = {
  0, 0, 1, 0, 1, 0, 0, 3,
  0, 0, 0, 0, 0, 0, 3, 3,
  0, 3, 0, 0, 0, 0, 0, 0,
  3, 0, 0, 1, 0, 1, 0, 1
};

static const char kTriCullBackHits_fix[] =  {0, 0, 0, 0, 0, 0, 0, 1};


void rastBBox_vec_fix( vector< u_Poly< long , ushort  > >& polys,
		      zbuff& z );

void rastBBox_bbox_fix( u_Poly< long , ushort >& poly , 
			       long& ll_x,
			       long& ll_y,
			       long& ur_x,
			       long& ur_y,
			       int&  ss_w_lg2, //log2 of subsample width
			       long& screen_w,
			       long& screen_h,
			       bool& valid ,
			       long r_shift, 
			       long r_val );


void rastBBox_uPoly_fix( u_Poly< long , ushort >& poly,
			 zbuff& z, 
			 long screen_w, 
			 long screen_h,
			 long si, //sample interval 
			 int  si_lg2, //log2 of sample interval
			 long r_shift, 
			 long r_val );


int rastBBox_stest_fix( u_Poly< long , ushort >& poly, long s_x , long s_y);

void rastBBox_jhash_jit_fix( 
			      const long& s_x,
			      const long& s_y,
			      const long& ss_w_lg2,
			      long* jitter_x,
			      long* jitter_y);
