#include "helper.h"

void abort_(const char * s, ...)
{
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}

uchar *blank( int w , int h )
{
  int x ;
  int y ;

  uchar* img ;
  uchar* rgba ;

  img = (uchar*) malloc( sizeof(uchar) * w * h * 3 );
  
  for( y=0 ;  y<h ; y++ ) {
    for( x=0 ; x<w ; x++ ) {
      rgba = &(img[(y*w+x)*3]);
      rgba[0] =  255 ; // Set R
      rgba[1] =  255 ; // Set G
      rgba[2] =  255 ; // Set B
    }
  }
  
  return img;
}

void rings( uchar* img, int w , int h )
{
  int x;
  int y;
  int rsq ;

  uchar* rgb ;

  for( y=0 ;  y<h ; y++ ) {
    for( x=0 ; x<w ; x++ ) {
      rgb = &( img[ (y*w+x)*3 ]  );
      rsq = x*x + y*y ; 
      if( 40 < rsq   && rsq < 300 ){ //Red Upper Left
	rgb[0] =  255 ; // Set R
	rgb[1] =  0 ;   // Set G
	rgb[2] =  0 ;   // Set B
      }

      rsq = ( w/2 - x ) * ( w/2 - x ) + ( h/2 - y ) * ( h/2 - y ) ;
      if( 40 < rsq   &&  rsq < 300 ){ //Green Center
	rgb[0] =  0 ;   // Set R
	rgb[1] =  255 ; // Set G
	rgb[2] =  0 ;   // Set B
      } 
     
      rsq = ( w - x ) * ( w - x ) + ( h - y ) * ( h - y ) ;
      if( 40 < rsq   &&  rsq < 300 ){ //Blue Lower Right
	rgb[0] =  0 ;   // Set R
	rgb[1] =  0 ;   // Set G
	rgb[2] =  255 ; // Set B
      } 

      rsq = ( w - x ) * ( w - x ) +  y * y ;
      if( 40 < rsq   &&  rsq < 300 ){ //Magenta Upper Right
	rgb[0] =  255 ; // Set R
	rgb[1] =  0 ;   // Set G
	rgb[2] =  255 ; // Set B
      } 

      rsq =  x * x + ( h - y ) * ( h - y ) ;
      if( 40 < rsq   &&  rsq < 300 ){ //Aqua Lower Left
	rgb[0] =  0 ;   // Set R
	rgb[1] =  255 ; // Set G
	rgb[2] =  255 ; // Set B
      } 

    }
  }

  return ;
}




/* File Format:
   /
   /   First Line: JB20
   /   Second Line: w,h,ss   
   /   
   /   One triangle or quad per line 
   /   Each Line:
   /    Number of Vertices
   /    For Each Vertex: 
   /       x,y,z   floats in screen space
   /       r,g,b,a floats [0,1]
*/

void read_in_vector( 
		    vector< u_Poly< long , ushort  > >& polys , 
		    char* file_name,
		    int* w,
		    int* h,
		    int* ss
		     )
{
  int i , j ;
  char buf[256];
  u_Poly<long , ushort > tmp ;
  int valid, integer;
  
  ifstream myfile (file_name);  /* Open File for Read */

  if( ! myfile.is_open() )
    abort_("Failed to Open Vector File for Read");

  /* Check First Line */
  myfile.getline( buf, 256 , '\n'); // First Line
  if( !strcmp( buf , "JB21" ) ){

    printf( "File type JB21 found, begin parsing\n");
		
    /* Grab Config From Second Line */
    myfile>>hex>>integer;
    *w = integer / 1024;

    myfile>>hex>>integer;
    *h = integer / 1024;

    myfile>>dec>>*ss;

    cout<<"Debug: \tw="<<*w<<" \th="<<*h<<" \tss="<<*ss<<endl;

    while( ! myfile.eof() ){
      myfile>>dec>>valid;
	
      //Get the Vertice Count
      myfile>>dec>>tmp.vertices; 
    
      if( tmp.vertices <3 || tmp.vertices > 4 ){
	//not safe, must guarentee input
	printf("End of File, %i\n" , tmp.vertices );
	return;
      }

      for( i = 0 ; i < 4 ; i++ ){
	//Vertice Screen Space Positions
	myfile>>hex>>tmp.v[i].x[0];
	myfile>>hex>>tmp.v[i].x[1];
	myfile>>hex>>tmp.v[i].x[2];
      }
	
      for( j = 0 ; j < 3 ; j++ )
	{ // Colors for vertice
	  myfile>>hex>>tmp.v[0].c[j];
	}
      tmp.v[0].c[3] = 1; // A=1.0
	  
      // copy the color of the first vertex to the others
      for( i = 1; i < 4 ; i++ ){
	for( j = 0 ; j < 4 ; j++ )
	  { 
	    tmp.v[i].c[j] = tmp.v[0].c[j] ;
	  }
      }
	  
      if(valid){
	polys.push_back(tmp);

      }

    }

  } else {

    printf( "%s\n" ,buf );
    abort_("File is Incorrect Format");
  
  }
  
  myfile.close();

}


void write_ppm_file( 
		    char* file_name , 
		    uchar* imgBuffer , 
		    int w , 
		    int h 
		     )
{
  /*Taken From: http://www.cse.ohio-state.edu/~shareef/cse681/labs/writePPM.html */

  FILE *stream;

  stream = fopen(file_name, "wb" );            // Open the file for write
  fprintf( stream, "P6\n%d %d\n255\n", w, h ); // Write the file header information
  
  for( int iy = 0; iy < h; iy++){             // Write the contents of the buffer to file
    for( int ix = 0; ix < w; ix++ ){

      // Access pixel (ix, iy) by indexing the appropriate row and then column in the
      // image buffer and taking into account that each pixel has
      // three unsigned char values. This command will write a single pixel value (rgb)
      // to the file in binary format.
      fwrite( imgBuffer + (iy * w + ix) * 3, 1, 3, stream );
    }
  }

  fclose( stream );

  return ;
}

