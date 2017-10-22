/*****************************************************************************
File: utils.cpp

Informatique Graphique
Master d'informatique
Christian Jacquemin, Universite Paris-Sud & LIMSI-CNRS

Copyright (C) 2014 University Paris-Sud 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#include "utils-KP-bones-modele.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

/////////////////////////////////////////////
// texture loading
bool loadtexture( string flieName , int index , bool isRect ) {
  // texture load through OpenCV
  image[index] = cv::imread( flieName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
  if(! image[index].data ) {                              // Check for invalid input
    printf(  "Could not open or find the image %s\n" , flieName.c_str() );
    return false;
  }
  printf(  "Loaded %s (%d channels)\n" , flieName.c_str() , image[index] .channels() );

  int colorTransform = (image[index] .channels() == 4) ? CV_BGRA2RGBA : 
    (image[index] .channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
  // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
  if( image[index] .channels() >= 3 ) {
    cv::cvtColor(image[index] , image[index] , colorTransform);
  }

  glEnable(GL_TEXTURE_2D);
  glGenTextures( 1, &(textureID[index]) );
  glActiveTexture (GL_TEXTURE0 + index);
  GLenum target = GL_TEXTURE_2D;
  if( isRect ) {
    target = GL_TEXTURE_RECTANGLE;
  }
  glBindTexture( target, textureID[index] );
  if( image[index] .channels() > 3 ) {
    glTexImage2D(target,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGBA8,            // Internal colour format to convert to
		 image[index].cols,          // Image width  i.e. 640 for Kinect in standard mode
		 image[index].rows,          // Image height i.e. 480 for Kinect in standard mode
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGBA, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_UNSIGNED_BYTE,  // Image data type
		 image[index].ptr());        // The actual image data itself
  }
  else {
    glTexImage2D(target,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGB,            // Internal colour format to convert to
		 image[index].cols,          // Image width  i.e. 640 for Kinect in standard mode
		 image[index].rows,          // Image height i.e. 480 for Kinect in standard mode
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGB, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_UNSIGNED_BYTE,  // Image data type
		 image[index].ptr());        // The actual image data itself
  }
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  // glGenerateMipmap(GL_TEXTURE_2D);
  return true;
}

////////////////////////////////////////
// prints errors after shader compiling
void printLog(GLuint obj)
{
  int infologLength = 0;
  int maxLength;
  
  if(glIsShader(obj))
    glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
  else
    glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
  
  char *infoLog = new char[maxLength];
  
  if (glIsShader(obj))
    glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
  else
    glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
  
  if (infologLength > 0)
    printf("%s\n",infoLog);
  
  delete infoLog;
}

////////////////////////////////////////
// shader file loading
unsigned long getFileLength(ifstream& file)
{
    if(!file.good()) return 0;
    
    file.seekg(0,ios::end);
    unsigned long len = file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

int loadshader(string filename, GLuint shader)
{
  GLchar* shaderSource;
  unsigned long len;

  ifstream file;
  file.open(filename.c_str(), ios::in); // opens as ASCII!
  if(!file) {
    printf( "Error: shader file not found %s!\n" , filename.c_str() ); throw 53;
  }
  
  len = getFileLength(file);
  if (len==0) {
    printf( "Error: empty shader file %s!\n" , filename.c_str() ); throw 53;
  }
  
  shaderSource = new char[len+1];
  if (shaderSource == 0)  {
    printf( "Error: shader string allocation error %s!\n" , filename.c_str() ); throw 53;
  }
  
  printf( "Loading %s\n" , filename.c_str() );

  // len isn't always strlen cause some characters are stripped in ascii read...
  // it is important to 0-terminate the real length later, len is just max possible value... 
  shaderSource[len] = 0; 
  
  unsigned int i=0;
  while (file.good()) {
    shaderSource[i] = file.get();       // get character from file.
    if (!file.eof())
      i++;
  }
  
  shaderSource[i] = 0;  // 0-terminate it at the correct position
  
  file.close();
  
  glShaderSource( shader, 1, (const char **)&shaderSource, NULL );
  
  delete[] shaderSource;
  
  return 0; // No Error
}

////////////////////////////////////////
// glfw callbacks for keystroke and error
void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode,
			 int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

//////////////////////////////////////////////////////////////////
// MATRIX ALGEBRA

double RealTime( void ) {
  struct timeval time;
  gettimeofday(&time, 0);
  return (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
}


