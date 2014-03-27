
#if !defined(J_ZBUFF)
#define J_ZBUFF

#include <vector>
#include <stdlib.h>

#include "helper.h"

class zbuff
{
 public:
  int w ;  // width
  int h ;  // height
  int ss ; // super sampling
  int ss_w ; // sqrt of super sampling
  int ss_w_lg2 ; // log 2 of the sqrt of sampling
  double ss_i ; // subsample interval

 private:
  ushort* frame_buffer ;
  uint*   depth_buffer ;

  uchar* eval_ss();
  void eval_ss( uchar* rgb , ushort* fb_pix);

 public:

  zbuff( int w_in , int h_in , int ss_in );

  void write_ppm( char* file_name );

  void process_Fragment( int x , int y , 
			 int ss_x , int ss_y ,
			 uint d ,
			 ushort R ,
			 ushort G ,
			 ushort B ,
			 ushort A  ) ;

};

#endif 
