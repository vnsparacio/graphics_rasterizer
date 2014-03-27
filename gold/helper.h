
/*
 *   Modified from http://www.zarb.org/~gc/html/libpng.html
 *
 *   A test of a simple function that can take an array and 
 *   write a png from it.
 *
 *
 *
 *
 */

/*
 * Copyright 2002-2008 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#if !defined( J_HELPER )
#define J_HELPER

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <fstream>
#include <string>

#include <vector>

using namespace std;

typedef unsigned char uchar ;
typedef unsigned short ushort ;
typedef unsigned int  uint;

template< typename T , typename S > struct uP_vertex {

  T x[3] ; // x y z
  S c[4] ; // R G B a 
};

template< typename T , typename S > struct u_Poly {
  int vertices ;
  uP_vertex< T , S > v[4] ;
}; 

struct fragment
{
  uint z ;
  ushort c[4] ;
};

void abort_(const char * s, ...);

uchar *blank( int w , int h );

void rings( uchar* img, int w , int h );

void read_in_vector( 
		    vector< u_Poly< long , ushort  > >& polys , 
		    char* file_name ,  
		    int* w,
		    int* h,
		    int* ss
		     );

void write_ppm_file( 
		    char* file_name , 
		    uchar* img , 
		    int w , 
		    int h 
		     );

#endif
