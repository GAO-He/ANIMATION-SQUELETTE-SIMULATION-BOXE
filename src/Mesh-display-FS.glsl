/*****************************************************************************
File: Mesh-display-FS.glsl

Virtual Humans
Master in Computer Science
Christian Jacquemin, University Paris-Sud

Copyright (C) 2014 University Paris-Sud 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris-Sud makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#version 130

// normal, texture coordinates, light in model coordinates
in vec3 normalOut;
in vec2 texCoordOut;
in vec3 lightOut;
// position in model coordinates
in vec3 positionOut;
// view in model coordinates
in vec3 eyeOut;

// uniforms
uniform vec3 objectColor;

// out color
out vec4 frag_colour;

void main () {
  // local view vector in model coordinates
  vec3 view = normalize(positionOut - eyeOut);
  
  // diffuse lighting contribution to color
  float diffuseFactor = max(0.0 , dot(normalOut,normalize(lightOut)) );
  float ambientFactor = 0.1;

  vec3 color;
  color =  vec3(diffuseFactor + ambientFactor) * objectColor;
   
  // specular lighting contribution to color
  if( objectColor.r == 1 ) {
      frag_colour = vec4(1,1,0,1.0);
  }
  else {
       frag_colour = vec4(color,1.0);
  }
}
