/*****************************************************************************
File: Mesh-display-VS.glsl

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

// input varyings
in vec3 vp;
in vec3 norm;
in vec2 texCoord;

// uniforms
uniform mat4x4 modelMatrix;
uniform mat4x4 inverseModelMatrix;
uniform mat4x4 viewMatrix;
uniform mat4x4 projectionMatrix;
uniform vec3 light;
uniform vec3 eye;

// outputs
// outputs normal and light for Phong shading

// normal, texture coordinates, light in model coordinates
out vec3 normalOut;
out vec2 texCoordOut;
out vec3 lightOut;
// position in model coordinates
out vec3 positionOut;
// view in model coordinates
out vec3 eyeOut;

void main () {
  // copies normals and passes them to FS
  normalOut = norm;
  // copies texture coordinates and passes them to FS
  texCoordOut = texCoord;

  // transformations of points (by adding a fourth 1.0 coordinates)
  // and vectors  (by adding a fourth 0.0 coordinates) through inverse
  // model transformation matrix
  // transforms light from world coordinates to model coordinates
  lightOut = (inverseModelMatrix * vec4(light,0.0)).xyz;
  // transforms eye position from world coordinates to model coordinates
  eyeOut = (inverseModelMatrix * vec4(eye,1.0)).xyz;


  // position in projection coordintates calculation for rasterizer
  gl_Position = projectionMatrix * viewMatrix * modelMatrix 
    * vec4 (vp, 1.0);
}
