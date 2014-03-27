EE271 Project, Fall 2011/10 - Test file format
===============================================

The test file contains polygons in text format according to the following rules:

Line 1 must specify the parameters for the screen: "Width Hight MSAA"
  "Width" and "Hight" are specified as 6 hexadecimal digits, 
  where r_shift=10 (i.e., 10 fractional bits). 
  MSAA can receive one of {1, 4, 16, 64}.
  The file sample_test.dat provides an example for a screen of 800 by 600 with MSAA=16

All other lines of the file, specify polygons according to the following format:
  "Valid #Vertices V0.x V0.y V1.x V1.y V2.x V2.y V3.x V3.y C.r C.g C.b"
  "Valid" may receive either 1 or 0
  "#Vertices" may receive either 3 (triangle) or 4 (quadrilateral)
  "V0.x" through "V3.y" are specified as 6 hexadecimal digits, 
  where r_shift=10 (i.e., 10 fractional bits). 
  "C.r" through "C.b" are also specified as 6 hexadecimial digits, but we only 
  use the four least significant digits, which means 00ffff is the largest 
  value for RGB chanel, corresponding to 0xff in 8bit color chanel.

The file sample_test.dat provides an example with simple input of only 
triangles, some are valid, some are not.



**** No other text is allowed in the file. 
**** No comments of any kind, and no missing lines.