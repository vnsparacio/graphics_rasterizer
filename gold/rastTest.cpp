
#include "zbuff.h"
#include "helper.h"
#include "rastBBox_fix.h"
#include <stdlib.h>
#include <stdio.h>


/*

   Test Rast
     This is a test function with a 
     few cases with known results.

     These tests should act as a rough
     test of whether the bounding box
     and sample test operation have 
     been configured correctly.

     This function also demonstrates
     how some of the primitives are defined.
*/
bool testRast()
{

  /* Lets Describe the Screen Space */
  long r_shift = 10 ;   //This is the number of fractional bits
  long r_val   = 1024 ; //This is the value of 1 << r_shift

  long screen_w = 1024 << r_shift ; // 1024 pixels wide (on x axis)
  long screen_h = 1024 << r_shift ; // 1024 pixels tall (on y axit)

  int ss_w_lg2 = 2 ; // This is the log_2 ( sqrt( MSAA ) )  

  u_Poly< long , ushort > poly; //This is a micropolygon
  poly.v[0].x[0] = 556 << ( r_shift - 2 ); //v0.x
  poly.v[0].x[1] = 679 << ( r_shift - 2 ); //v0.y

  poly.v[1].x[0] = 562 << ( r_shift - 2 ); //v1.x
  poly.v[1].x[1] = 660 << ( r_shift - 2 ); //v1.y

  poly.v[2].x[0] = 557 << ( r_shift - 2 ); //v2.x
  poly.v[2].x[1] = 661 << ( r_shift - 2 ); //v2.y

  poly.v[3].x[0] = 561 << ( r_shift - 2 ); //v3.x
  poly.v[3].x[1] = 680 << ( r_shift - 2 ); //v.y

  poly.vertices = 3; //The number of vertices
                     // Three is a triangle
                     // Four is a quadrilateral
  // Note that there are other parameters in
  // a micropolygon, but they are more or less
  // irrelevant for this portion of the algorithm

  long ll_x;
  long ll_y;
  long ur_x;
  long ur_y;
  bool valid;

  long s_x , s_y ;
  int hit ;

  printf( "Test 1: Bounding Box Test\n" );
  poly.vertices = 3;

  rastBBox_bbox_fix(  poly, ll_x, ll_y, ur_x, ur_y, ss_w_lg2, 
		      screen_w, screen_h, valid, r_shift, r_val);

  if( ! valid ){
    abort_("Fail Test 1"); 
  }
  if( ll_x != (556 << ( r_shift - 2 )) ){
    abort_("Fail Test 1"); 
  }
  if( ll_y != (660 << ( r_shift - 2 )) ){
    abort_("Fail Test 1"); 
  }
  if( ur_x != (562 << ( r_shift - 2 )) ){
    abort_("Fail Test 1"); 
  }
  if( ur_y != (679 << ( r_shift - 2 )) ){
    abort_("Fail Test 1"); 
  }

  printf( "\t\tPass Test 1\n");


  /* 
     If you are having trouble determining if your bounding
     box function is correct, you can add more test cases
     here.
  */

  printf( "Test 2: SampleTest Test\n" );
  poly.vertices = 3;

  s_x = 559 << ( r_shift - 2 );
  s_y = 662 << ( r_shift - 2 );
  hit = rastBBox_stest_fix( poly, s_x , s_y);

  if( hit == -1 ) { //If a miss
    abort_("Failed Test 2");
  }

  printf( "\t\tPass Test 2\n");


  printf( "Test 3: SampleTest Test\n" );
  poly.vertices = 3;

  s_x = 560 << ( r_shift - 2 );
  s_y = 678 << ( r_shift - 2 );
  hit = rastBBox_stest_fix( poly, s_x , s_y);

  if( hit!=-1 ) { //If a hit
    abort_("Failed Test 3");
  }

  printf( "\t\tPass Test 3\n");

  /* 
     If you are having trouble determining if your sample test
     function is correct, you can add more test cases
     here.
  */

  return true ;
}




int main(int argc, char **argv)
{

  if( ! testRast() )
  {
    abort_("Test Failed");
  }

  if (argc != 3)
  {
    abort_("Usage: program_name <file_out> <vector>");
  }
  
  //Set Screen and Subsample
  int w  ;
  int h ;
  int ss ; 
  
  //Initialize the polygon soup
  vector< u_Poly< long , ushort > > polys ;

  //Read in the list of polygons
  read_in_vector( polys , argv[2] ,&w , &h , &ss );

  int size_s = (int)polys.size();

  //Report Number of polygons
  printf( "%i\n" , size_s );
  
  //Initialize a Depth Buffer
  zbuff z(w,h,ss);

  //Rasterize the Scene
  rastBBox_vec_fix( polys , z );

  //Write the Zbuffer to a png
  z.write_ppm( argv[1] );

  return 0;
}
