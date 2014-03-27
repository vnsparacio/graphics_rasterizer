/*
 *
 *  C Code for SV - DPI  
 *
 *
 *
 *
 *
 */

//DPI Includes
#include "acc_user.h"
#include "vcs_acc_user.h"
#include "svdpi.h"
#include "stdlib.h"

typedef unsigned char uchar ;
typedef unsigned short ushort ;
typedef unsigned int  uint;

const char kQuadCullBackHits_fix[] = {
  0, 0, 1, 0, 1, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 1, 1,
  0, 1, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 1, 0, 1, 0, 1
};

const char kTriCullBackHits_fix[] =  {0, 0, 0, 0, 0, 0, 0, 1};

uint*    depthBuffer;
ushort*  frameBuffer;
int wB, hB, sswB, ssB;

typedef struct { // uP_vertex 
  int x[2] ; // x y z
} uP_vertex ;

typedef struct { //u_Poly 
  int q;
  uP_vertex v[4] ;
} u_Poly ; 

//Given a uPoly and Bbox check that Bbox is good bound 
// on uPoly
int rastBBox_bbox_check( int   v0_x,     //uPoly
			 int   v0_y,     //uPoly
			 int   v1_x,     //uPoly
			 int   v1_y,     //uPoly
			 int   v2_x,     //uPoly
			 int   v2_y,     //uPoly
			 int   v3_x,     //uPoly
			 int   v3_y,     //uPoly
			 int   q,        //uPoly
                         int  valid_Poly,
			 int   ll_x,     //BBOX
			 int   ll_y,     //BBOX
			 int   ur_x,     //BBOX
			 int   ur_y,     //BBOX
			 int   ss_w_lg2, //Subsample
			 int   screen_w, //Screen
			 int   screen_h, //Screen
			 int   valid ,   //BBoX
			 int   r_shift,  //Config
			 int   r_val     //Congig 
			 );

//Given a uPoly and a sample location check that hit agrees with
// sample lieing in/out of uPoly.
// Return 0 if disagree
// Return 1 if agree
int rastBBox_stest_check( int   v0_x,      //uPoly
			  int   v0_y,      //uPoly
			  int   v1_x,      //uPoly
			  int   v1_y,      //uPoly
			  int   v2_x,      //uPoly
			  int   v2_y,      //uPoly
			  int   v3_x,      //uPoly
			  int   v3_y,      //uPoly
			  int   q,         //uPoly
			  int   s_x,       //SAMPLE 
			  int   s_y,       //SAMPLE
			  int   hit        //HIT
			  );   


//Given a uPoly determine if the hits argument 
// is equivelant to the number of actual hits
int rastBBox_check( int   v0_x,      //uPoly
		    int   v0_y,      //uPoly
		    int   v1_x,      //uPoly
		    int   v1_y,      //uPoly
		    int   v2_x,      //uPoly
		    int   v2_y,      //uPoly
		    int   v3_x,      //uPoly
		    int   v3_y,      //uPoly
		    int   q,         //uPoly
		    int   hits,      //Number of Samples in uPoly
		    int   ss_w_lg2,  //Subsample
		    int   screen_w,  //Screen
		    int   screen_h,  //Screen
		    int   r_shift,   //Config
		    int   r_val      //Congig 
		     );   


int zbuff_init( int w,
		int h,
		int ss_w 
		); 

int zbuff_rop(  int x , 
		int y , 
		int ss_x , 
		int ss_y ,
		int d , //actually a uint
		int R , //actually a ushort
		int G , //actually a ushort
		int B   //actually a ushort
		) ;

int write_ppm( );

uchar *blank( int w , int h );

void write_ppm_file( 
		    char* file_name , 
		    uchar* img , 
		    int w , 
		    int h 
		     );


void rastBBox_jhash_jit_fix( 
			      long  s_x,
			      long  s_y,
			      long  ss_w_lg2,
			      ushort* jitter_x,
			      ushort* jitter_y);


int rastBBox_jhash_jit_fix_check( 
			      int s_x,
			      int s_y,
			      int ss_w_lg2,
			      int jitter_x,
			      int jitter_y,
			      int s_j_x,
			      int s_j_y );
