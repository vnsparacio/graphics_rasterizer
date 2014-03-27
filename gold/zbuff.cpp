

#include "zbuff.h" 
#include "float.h"
#include "limits.h"
#include "assert.h"

#define  IDX_F( x , y , sx , sy , c ) \
  ((((( y*w ) + x)*ss_w+sy)*ss_w+sx)*4+c)

#define  IDX_D( x , y , sx , sy ) \
  (((( y*w ) + x)*ss_w+sy)*ss_w+sx)

// Build a black zbuffer
zbuff::zbuff( int w_in , int h_in , int ss_in )
{

  int i ;

  w = w_in ;
  h = h_in ; 
  ss = ss_in ; 

  switch( ss )
  {
  case 1:  ss_w = 1 ;  ss_w_lg2 = 0 ; break ;
  case 4:  ss_w = 2 ;  ss_w_lg2 = 1 ; break ;
  case 16: ss_w = 4 ;  ss_w_lg2 = 2 ; break ;
  case 64: ss_w = 8 ;  ss_w_lg2 = 3 ; break ;
  }

  ss_i = 1024 / ss_w ;

  frame_buffer = (ushort*)malloc( sizeof( ushort ) * w_in * h_in * ss * 4 ) ;
  depth_buffer = (uint*)malloc( sizeof( uint ) *  w_in * h_in * ss ) ; 

  for( i = 0 ; i < ( w_in * h_in * ss * 4 ) ; i++ ){
    frame_buffer[i] = 0 ;
  }

  for( i = 0 ; i < ( w_in * h_in * ss ) ; i++ ){
    depth_buffer[i] = UINT_MAX ;
  }

}

// Evaluate the Subsamples at the given pixel
//  return the colors for that fragment
void zbuff::eval_ss( uchar* rgb , ushort* fb_pix)
{
  int i , j , k;

  uint rgb_l[4];
  rgb_l[0] = 0 ;
  rgb_l[1] = 0 ;
  rgb_l[2] = 0 ;

  for( i = 0 ; i < ss_w ; i++ )
    for( j = 0 ; j < ss_w ; j++ )
      for( k = 0 ; k < 4 ; k++ )
	rgb_l[k] += fb_pix[ (i*ss_w+j)*4 + k ];
      

  for( k = 0 ; k < 3 ; k++ )
    rgb[k] = (uchar) (( rgb_l[k] / ss ) >> ( 8 )) ;
  
}

// Evaluate All Subsamples
uchar* zbuff::eval_ss(){

  int x ;
  int y ;

  uchar* rgb ;
  ushort* fb_pix ;

  uchar* img = blank(w,h);
  
  for( y=0 ;  y<h ; y++ ) {
    for( x=0 ; x<w ; x++ ) {
      rgb = &(img[ (y*w + x)*3]);
      fb_pix = &( frame_buffer[ IDX_F( x , y , 0 , 0 , 0 ) ] ) ;
      eval_ss( rgb , fb_pix ) ;
    }
  }

  return img ;
}

void zbuff::write_ppm( char* file_name ){ 

  uchar* img = eval_ss();
  write_ppm_file( file_name , img  , w , h );

}

void zbuff::process_Fragment( int x , int y , 
			      int ss_x , int ss_y,
			      uint d ,
			      ushort R ,
			      ushort G ,
			      ushort B ,
			      ushort A ) {


  if( d <= depth_buffer[ IDX_D( x, y, ss_x, ss_y ) ] ){
    depth_buffer[ IDX_D( x, y, ss_x, ss_y ) ] = d ;
    uint id = IDX_F( x, y, ss_x, ss_y, 0 ) ;
    frame_buffer[ id ] = R ;
    frame_buffer[ id + 1 ] = G ;
    frame_buffer[ id + 2 ] = B ;
    frame_buffer[ id + 3 ] = A ;
  }
}


