/*****************************************************************************
File: KP-anim.cpp

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
#include "utils-KP-bones-modele.h"
#include "udp.h"
#include <math.h>

// screen size
int height = 1080;
int width = 1920;
float near = 1.0f;

// perspective projection
bool perspective = false;

// mesh modification
#define ANIMATION_STEP    0.001
#define ANGULAR_STEP    1
#define PI 3.14159265359
#define G  1.8
#define L  1

using namespace std;
// initial time
double InitialTime;

// texture
cv::Mat image[1];
GLuint textureID[1];


//frame count
long NbFrames = 0;

// light position
GLfloat lightPosition[3] = { 20.0f , 0.0f , 40.0f };

// eye
GLfloat eyePosition[3] = { 2.0f , 0.0f , 5.2f };

// mouse control
double mouse_pos_x = 0, mouse_pos_y = 0;
float DistanceFactor = 1.0;

// the current object
Object *bodyObject;
Object *boxObject;

// the current object
Shader *bodyShader;

// statut
bool ready = false;                                 //le statut d'humain:préparation
bool boxe_gauche = false;
bool boxe_droite = false;
bool sacFrappe = false;                             //le statut de la sac de frappe
bool marche = false;
bool collision = false;                             //rencontre entre la sac de frappe et la main

double boxe_vit_x;                                  // le vitesse de la sac de frappe
double boxe_vit_y;
double boxe_vit_z;
double boxe_theta_max = 5/180*PI;                   // l'angle d'osci maximun de la sac
double Ym = 30;                                     // l'amplitude des osci
double Zm = 30;
int T_boxe = 2;                                     // la période de la sac

float t_sacFrappe;                                  // le temps de départ pour la sac
float t_ready;                                      // le temps de préparation
float t_boxe_gauche;                                // le temps pour la sac aller à gauche
float t_boxe_droite;

// function prototypes
int main(int argc, char **argv);
void translation_boxe_gauche (double t0);           // l'action pour la sac aller à gauche
void translation_boxe_droite (double t0);
void translation_ready(double t0);
//void box_interp_geste(double t0);
void box_animation(double t0);
//double getTheta();

float linear_weight( float distance , float radius , int exponent );


//////////////////////////////////////////////////////////////////
// ANIMATION
//////////////////////////////////////////////////////////////////

char tab_ID[4][16] = { "8.1" , "8.2" , "8.3" , "8.4" };
float tab_KF_x[4][10] = {
    { 0.01 , 0.02 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.03 , 0.04 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.0 , 0.0 , 0.05 , 0.06 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.07 , 0.08 , 0.0 , 0.0 }
  };
float tab_KF_y[4][10] = {
    { 0.09 , 0.08 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.07 , 0.06 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.0 , 0.0 , 0.05 , 0.04 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.03 , 0.02 , 0.0 , 0.0 }
  };
float tab_KF_z[4][10] = {
    { 0.04 , 0.05 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.07 , 0.09 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.0 , 0.0 , 0.01 , 0.03 , 0.0 , 0.0 , 0.0 , 0.0 }  ,
    { 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.02 , 0.08 , 0.0 , 0.0 }
  };

char tab_Bones_ID[8][21] = { "Bra_D" , "AvantBra_D" , "Bra_G" , "AvantBra_G" ,"Femur_D" , "Tibia_D", "Femur_G" , "Tibia_G"  };
int tab_Bones_indices[8] = { -1 , -1 , -1 , -1 , -1 , -1, -1 , -1  };
float tab_Bones_KF_a[8][21] = {
    //   0        1        2        3       4        5         6        7        8        9       10       11       12       13       14        15      16       17        18       19       20
    {  1.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 ,-25.000 , 56.566 ,100.000 , 56.566 , 16.000 ,-25.000 , 1.000 ,-25.000 ,-25.000 ,-25.000 , -25.000 }  ,
    {  1.000 , 40.000 , 40.000 , 40.000 , 40.000 , 82.000 , 82.000 , 82.000 , 82.000 , 82.000 , 82.000 , 35.000 ,  0.000 , 35.000 , 58.000 , 82.000 , 1.000 , 82.000 , 82.000 , 82.000 ,  82.000 }  ,

    {  1.000 , 20.000 , 20.000 , 20.000 , 20.000 , 20.000 ,-46.941 ,-100.000,-46.941 , 20.000 , 20.000 , 20.000 , 20.000 , 20.000 , 20.000 , 20.000 ,  1.000 , 20.000 , 20.000 , 20.000 , 20.000 }  ,
    {  1.000 ,-40.000 ,-40.000 ,-40.000 ,-40.000 ,-78.000 ,-39.000 ,  0.000 ,-39.000 ,-78.000 ,-78.000 ,-78.000 ,-78.000 ,-78.000 ,-78.000 ,-78.000 ,  1.000 ,-78.000 ,-78.000 ,-78.000 ,-78.000 }  ,

    {  0.000, -10.000 ,  0.000 , 10.000 ,  1.000 ,-40.000 ,-40.000 ,-40.000 ,-40.000 ,  0.000 , 18.000 , 18.000 , 18.000 , 18.000 ,  9.000 ,  0.000 ,  1.000 ,-40.000 ,  0.000 , 18.000 ,  0.000 }  ,
    {  0.000 ,-10.000 ,  0.000 , 10.500 ,  1.000 ,-45.000 ,-45.000 ,-45.000 ,-45.000 ,  0.000 , -0.500 , -0.500 , -0.500 , -0.500 , -0.250 ,  0.000 ,  1.000 ,-45.000 ,  0.000 , -0.500 ,  0.000 }  ,

    {  0.000 ,-10.000 ,  0.000 , 10.000 ,  1.000 ,-24.000 ,-24.000 ,-24.000 ,-24.000 ,  1.000 , 44.000 , 44.000 , 44.000 , 44.000 , 22.000 ,  1.000 ,  1.000 ,-24.000 ,  1.000 , 44.000 ,  1.000 }  ,
    {  0.000 ,-10.000 ,  0.000 , 10.000 ,  1.000 , -3.000 , -3.000 , -3.000 , -3.000 ,  0.000 , 41.000 , 41.000 , 41.000 , 41.000 , 22.000 ,  0.000 ,  1.000 , -3.000 ,  0.000 , 41.000 ,  0.000 }
 };
float tab_Bones_KF_x[8][21] = {
    //   0        1        2        3       4        5         6        7        8        9       10       11       12       13       14        15      16       17        18       19       20
    {  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.025 ,  0.050 ,  0.025 ,  0.000 ,  0.000 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 }  ,
    {  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 }  ,

    {  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 , -0.060 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 }  ,
    {  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 }  ,

    {  1.000 , -0.037 ,  1.000 , -0.037 ,  1.000 , -0.037 , -0.037 , -0.037 , -0.037 ,  1.000 , -0.037 , -0.037 , -0.037 , -0.037 ,  0.500 ,  1.000 ,  1.000 , -0.037 ,  1.000 , -0.037 ,  1.000 }  ,
    {  1.000 , -0.810 ,  1.000 ,  0.000 ,  1.000 , -0.810 , -0.810 , -0.810 , -0.810 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.500 ,  1.000 ,  1.000 , -0.810 ,  1.000 ,  0.000 ,  1.000 }  ,

    {  1.000 ,  0.004 ,  1.000 ,  0.000 ,  1.000 ,  0.004 ,  0.004 ,  0.004 ,  0.004 ,  1.000 ,  0.021 ,  0.021 ,  0.021 ,  0.021 ,  0.500 ,  1.000 ,  1.000 ,  0.004 ,  1.000 ,  0.021 ,  1.000 }  ,
    {  1.000 ,  0.990 ,  1.000 ,  0.000 ,  1.000 ,  0.990 ,  0.990 ,  0.990 ,  0.990 ,  1.000 ,  0.725 ,  0.725 ,  0.725 ,  0.725 ,  0.870 ,  1.000 ,  1.000 ,  0.990 ,  1.000 ,  0.725 ,  1.000 }
};
float tab_Bones_KF_y[8][21] = {
    //   0        1        2        3       4        5         6        7        8        9       10       11       12       13       14        15      16       17        18       19       20
    {  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.009 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 }  ,
    {  0.000 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.142 ,  0.072 ,  0.142 ,  0.000 ,  0.100 ,  0.100 ,  0.100 ,  0.100 }  ,

    {  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.011 ,  0.021 ,  0.011 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  1.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 }  ,
    {  0.000 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.200 ,  0.400 ,  1.000 ,  0.400 ,  0.400 ,  0.400 ,  0.400 }  ,

    {  0.000 ,  0.015 ,  0.000 ,  0.015 ,  0.000 ,  0.015 ,  0.015 ,  0.015 ,  0.015 ,  0.000 ,  0.015 ,  0.015 ,  0.015 ,  0.015 ,  0.007 ,  0.000 ,  1.000 ,  0.015 ,  0.000 ,  0.015 ,  0.000 }  ,
    {  0.000 ,  0.169 ,  0.000 ,  0.000 ,  0.000 , -0.169 , -0.169 , -0.169 , -0.169 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  0.000 ,  1.000 , -0.169 ,  0.000 ,  0.000 ,  0.000 }  ,

    {  0.000 , -0.025 ,  0.000 , -0.000 ,  0.000 , -0.025 , -0.025 , -0.025 , -0.025 ,  0.000 , -0.019 , -0.019 , -0.019 , -0.019 , -0.040 ,  0.000 ,  1.000 , -0.025 ,  0.000 , -0.019 ,  0.000 }  ,
    {  0.000 ,  0.113 ,  0.000 , -0.000 ,  0.000 ,  0.113 ,  0.113 ,  0.113 ,  0.113 ,  0.000 , -0.037 , -0.037 , -0.037 , -0.037 , -0.070 ,  0.000 ,  0.000 ,  0.113 ,  0.000 , -0.037 ,  0.000 }
 };
float tab_Bones_KF_z[8][21] = {
    //   0        1        2        3       4        5         6        7        8        9       10       11       12       13       14        15      16       17        18       19       20
    {  0.000 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.053 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.000 ,  0.047 ,  0.047 ,  0.047 ,  0.047 }  ,
    {  0.000 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.400 ,  0.248 ,  0.248 ,  0.248 ,  0.248 ,  0.248 ,  0.248 ,  0.000 ,  0.400 ,  0.400 ,  0.400 ,  0.400 }  ,

    {  0.000 ,  0.070 ,  0.070 ,  0.070 ,  0.070 ,  0.070 ,  0.082 ,  0.093 ,  0.082 ,  0.070 ,  0.070 ,  0.070 ,  0.070 ,  0.070 ,  0.700 ,  0.070 ,  1.000 ,  0.070 ,  0.070 ,  0.070 ,  0.070 }  ,
    {  0.000 ,  0.100 ,  0.100 ,  0.100 ,  0.100 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  0.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 }  ,

    {  0.000 ,  0.047 ,  0.000 ,  0.047 ,  0.000 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.000 ,  0.047 ,  0.047 ,  0.047 ,  0.047 ,  0.023 ,  0.000 ,  0.000 ,  0.047 ,  0.000 ,  0.047 ,  0.000 }  ,
    {  0.000 ,  0.605 ,  0.000 ,  1.000 ,  0.000 ,  0.605 ,  0.605 ,  0.605 ,  0.605 ,  0.000 ,  1.000 ,  1.000 ,  1.000 ,  1.000 ,  0.500 ,  0.000 ,  0.000 ,  0.605 ,  0.000 ,  1.000 ,  0.000 }  ,

    {  0.000 ,  0.317 ,  0.000 ,  0.127 ,  0.000 ,  0.317 ,  0.317 ,  0.317 ,  0.317 ,  0.000 ,  0.127 ,  0.127 ,  0.127 ,  0.127 ,  0.060 ,  0.000 ,  0.000 ,  0.317 ,  0.000 ,  0.127 ,  0.000 }  ,
    {  0.000 ,  0.633 ,  0.000 ,  0.346 ,  0.000 ,  0.633 ,  0.633 ,  0.633 ,  0.633 ,  0.000 ,  0.346 ,  0.346 ,  0.346 ,  0.346 ,  0.170 ,  0.000 ,  0.000 ,  0.633 ,  0.000 ,  0.346 ,  0.000 }
 };
//////////////////////////////////////////////////////////////////
// KEYPOINT AND POINT WEIGHTING
//////////////////////////////////////////////////////////////////

// a keypoint weighting scheme: linear weighting

float linear_weight( float distance , float radius , int exponent ) {
  if( distance < radius ) {
    return (radius - distance) / radius;
  }
  else {
    return 0.f;
  }
}

//////////////////////////////////////////////////////////////////
// INTERACTION
//////////////////////////////////////////////////////////////////

////////////////////////////////////////
// keystroke interaction
void char_callback(GLFWwindow* window, unsigned int key)
{
  switch (key) {
  case '<':
    DistanceFactor *= 1.1;
    break;
  case '>':
    DistanceFactor /= 1.1;
    break;
  case '+':
    eyePosition[2] += 0.1;
    break;
  case '-':
    eyePosition[2] -= 0.1;
    break;
  case '1':
    bodyObject->CurrentActiveKeyPoint = 0;
    bodyObject->CurrentActiveBone = 0;
    break;
  case '2':
    bodyObject->CurrentActiveKeyPoint = 1;
    bodyObject->CurrentActiveBone = 1;
    break;
  case '3':
    bodyObject->CurrentActiveKeyPoint = 2;
    bodyObject->CurrentActiveBone = 2;
    break;
  case '4':
    bodyObject->CurrentActiveKeyPoint = 3;
    bodyObject->CurrentActiveBone = 3;
    break;
  case '5':
    bodyObject->CurrentActiveKeyPoint = 4;
    bodyObject->CurrentActiveBone = 4;
    break;
  case '6':
    bodyObject->CurrentActiveKeyPoint = 5;
    bodyObject->CurrentActiveBone = 5;
    break;
  case '7':
    bodyObject->CurrentActiveKeyPoint = 6;
    bodyObject->CurrentActiveBone = 6;
    break;
  case '8':
    bodyObject->CurrentActiveKeyPoint = 7;
    bodyObject->CurrentActiveBone = 7;
    break;
  case '9':
    bodyObject->CurrentActiveKeyPoint = 8;
    bodyObject->CurrentActiveBone = 8;
    break;
/*  case 'X':
    bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.x
      += ANIMATION_STEP;
    break;
  case 'x':
    bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.x
      -= ANIMATION_STEP;
    break;

  case 'Y':
    bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.y
      += ANIMATION_STEP;
    break;
  case 'y':
    bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.y
      -= ANIMATION_STEP;
    break;
  case 'Z':
    bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.z
      += ANIMATION_STEP;
    break;
  case 'z':
    bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.z
      -= ANIMATION_STEP;
    break;*/
  case 'l':
    lightPosition[1] -= 1;
    break;
  case 'L':
    lightPosition[1] += 1;
    break;


  // la translation de personne
  case 'z':
    bodyObject->objectTranslation[0] -= 0.11;
    bodyObject->objectAngle_z = -150.0;
    bodyObject->direction = 't';
    marche = true;
    ready = false;
    boxe_gauche = false;
    boxe_droite = false;
    break;
  case 's':
    bodyObject->objectTranslation[0] += 0.11;
    bodyObject->objectAngle_z = 170.0;
    bodyObject->direction = 'b';
    marche = true;
    ready = false;
    boxe_gauche = false;
    boxe_droite = false;
    break;
  case 'q':
    bodyObject->objectTranslation[1] -= 0.11;
    bodyObject->objectAngle_z = 0.0;
    bodyObject->direction = 'l';
    marche = true;
    ready = false;
    boxe_gauche = false;
    boxe_droite = false;
    break;
  case 'd':
    bodyObject->objectTranslation[1] += 0.11;
    bodyObject->objectAngle_z = 320.0;
    bodyObject->direction = 'r';
    marche = true;
    ready = false;
    boxe_gauche = false;
    boxe_droite = false;
    break;
  // la translation de boxe
  case 'r':
    marche = false;
    ready = true;
    boxe_gauche = false;
    boxe_droite = false;
    t_ready = RealTime()-InitialTime;
    break;

  case 'g':
    marche = false;
    ready = false;
    boxe_gauche = true;
    boxe_droite = false;
    t_boxe_gauche = RealTime() - InitialTime;
    break;

  case 'h':
    marche = false;
    ready = false;
    boxe_gauche = false;
    boxe_droite = true;
    t_boxe_droite = RealTime() - InitialTime;
    break;

  case 'b':
    sacFrappe = true;
    t_sacFrappe = RealTime() - InitialTime;
    break;

  default:
    printf ("La touche %d n´est pas active.\n", key);
    break;
  }
}

////////////////////////////////////////
// mouse interaction
bool drag = false;

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (!drag && action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {          // la fonction pour controler la rotation d'humain
                                                                                    //明天删除？
    double x = 0;
    double y = 0;
    glfwGetCursorPos (window, &x , &y);
    mouse_pos_x = x;
    mouse_pos_y = y;
    drag = true;
  }
  if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    drag = false;
  }
}


////////////////////////////////////////
// UDP message processing

void processUDPMessages( void ) {
  // reads incoming UDP messages
  if( SocketToLocalServer >= 0 ) {
    char    message[1024];
    // init message buffer: Null values
    memset(message,0x0,1024);

    // receive message
    int n = recv(SocketToLocalServer, message, 1024, 0);
    if( n >= 0 ) {
      // scans the message and extract string & float values
      char MessID[512];
      float MessVector[4];
      int keyFrame;
      printf( "Message size %d\n" , n );
      printf( "Rec.: [%s]\n" , message );

      // receives a string of format "%s %d %f %f %f"
      // built from: keypoint ID, no of Keyframe, translation of keypoint
      // scan the message and assign keypoints with corresponding translation
      sscanf( message , "%s %d %f %f %f %f" , MessID , &keyFrame ,
	      MessVector , MessVector + 1 , MessVector + 2 , MessVector + 3 );
      printf( "ID [%s] KF [%d] rot (%.3f,%.3f,%.3f,%.3f)\n" , MessID , keyFrame ,
	      MessVector[0] , MessVector[1] , MessVector[2] , MessVector[3] );

      // find current keypoint index from keypoint ID
      bodyObject->CurrentActiveKeyPoint = bodyObject->KeypointIndex( MessID );

      if( bodyObject->CurrentActiveKeyPoint >= 0 ) {
	// assigns the translation to CurrentActiveKeyPoint
	bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.x
	  = MessVector[0];
	bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.y
	  = MessVector[1];
	bodyObject->TabKPs[bodyObject->CurrentActiveKeyPoint].translation.z
	  = MessVector[2];
	return;
      }

      // find current keypoint index from keypoint ID
      bodyObject->CurrentActiveBone = bodyObject->BoneIndex( MessID );

      if( bodyObject->CurrentActiveBone >= 0
	  && (MessVector[1] || MessVector[2] || MessVector[3]) ) {
	glm::vec3 axis = glm::normalize( glm::vec3( MessVector[1] ,
						    MessVector[2] ,
						    MessVector[3] ) );
	float angle = M_PI * MessVector[0] / 180.0;

	// transforms the rotation in a 4x4 matrix
	bodyObject->TabBones[bodyObject
		   ->CurrentActiveBone].boneAnimationRotationMatrix
	  = glm::rotate( glm::mat4(1.0f), angle , axis );

	return;
     }

      printf("UDP message with unknown Bone or KeyPoint (%s)\n" , MessID );
      return;

    }
  }
}
//compléter un mouvement
void KF_interp_geste( int geste[], int taille, float duree, float T0 ){

    float t = RealTime() - InitialTime;            //

    float temps_elem = duree/(taille-1);        //durée élémentaire = chaque interval entre deux keyframe va utiliser combien de temps
    if((t - T0) <= duree && t >= T0){               //vérifier que le "time" est inférieur que la durée total et supérieur que l'heure de début
        float t_normalize = (t - T0)/temps_elem;     //nb d'interval = durée total/le temps que chaque interval utilise
        int Id = (int)floor(t_normalize)%(taille-1);     //indice de départ = (nb d'intervals) mod (tailles de tableau de geste)
        int Ia = (Id + 1)%(taille);                    //indice d'arrivé
        int Ipd = geste[ Id ];               //indice de keyframe utilisé pour le départ
        int Ipa = geste[ Ia ];               //indice de keyframe utilisé pour l'arrivée
        float alpha = t_normalize - (int)floor(t_normalize);

        for( int ind = 0 ; ind < 8 ; ind++ ) {
            int boneIndex = tab_Bones_indices[ind];

        /////////////////////////////// TODO ///////////////////////////
        // transforms into quaternions the indKF and indNextKF
        // angle/axis values of the 4 posed bones
            glm::quat quat1; // quaternion of indKF pose
            glm::quat quat2; // quaternion of indNextKF pose

            quat1 = glm::angleAxis(
                (float) (tab_Bones_KF_a[ind][Ipd] * M_PI / 180.0F),
                glm::normalize(glm::vec3(
                    tab_Bones_KF_x[ind][Ipd],
                    tab_Bones_KF_y[ind][Ipd],
                    tab_Bones_KF_z[ind][Ipd])));

            quat2 = glm::angleAxis(
                (float) (tab_Bones_KF_a[ind][Ipa] * M_PI / 180.0F),
                glm::normalize(glm::vec3(
                    tab_Bones_KF_x[ind][Ipa],
                    tab_Bones_KF_y[ind][Ipa],
                    tab_Bones_KF_z[ind][Ipa])));

            glm::quat quatInterp = glm::mix(quat1, quat2, alpha);
            bodyObject->TabBones[boneIndex].boneAnimationRotationMatrix =
                glm::mat4_cast(quatInterp);

    /////////////////////////////// TODO ///////////////////////////
    // assigns to boneAnimationRotationMatrix of bone no boneIndex
    // the mix of quat1 and quat2 by coeficient alpha
        }
    }
}

// pour n fois
void KF_repet_geste( int geste[], int taille, float duree, float T0, int n ){

    for( int i = 0; i < n; i ++){
        float duree_1 = duree/n;           //La durée pour chaque action
        float T0_i = T0 + duree_1 * i;
        KF_interp_geste(geste, taille, duree_1, T0_i);
    }

}

//Translation
void translation_ready (double t0){                     //  répéter la fonction de préparation
    // to signefie le tmp initial

    int gestion_ready[ 9 ] = {  0, 1, 2, 3, 4, 3, 2, 1,0  };

    KF_repet_geste(gestion_ready, 9, 4000, t0, 1000);

}

void translation_boxe_gauche (double t0){                    //  faire de la boxe par la main gauche une fois

    int gestion_boxe[ 7 ] = { 1, 5, 6, 7, 8 , 9 ,1 };

    KF_interp_geste(gestion_boxe, 7, 4, t0);
}


void translation_boxe_droite (double t0){                    // faire de la boxe par la main droite

    int gestion_boxe[ 8 ] = { 1, 10, 11, 12, 13, 14, 15, 1 };

    KF_interp_geste(gestion_boxe, 8, 5, t0);
}

void translation_marche(){
    int gestion_marche[ 5 ] = {16, 17, 18, 19, 20 };

    KF_repet_geste( gestion_marche, 5, 5000, 0, 1000 );
}


/*
void box_interp_geste(int geste[], int taille, double duree, double t0) {

    double t = RealTime() - InitialTime;

    double duree_element = duree/(taille - 1);

    if( (t-t0) <= duree && t >= t0 ){

        float t_normalise = (t - t0) / duree_element;

        int indKF = (int) floor( t_normalise ) % (taille - 1);
        int indNextKF = (indKF + 1) % taille;

        int Ipd = geste[indKF];
        int Ipa = geste[indNextKF];

        float alpha = t_normalise - floor( t_normalise );

        // box modele matrix calculation
        // movement de la box
        boxObject->objectAngle_x = 90;
        boxObject->objectAngle_y = 90;
        boxObject->objectAngle_z = 45;


        boxObject->objectTranslation[0] = (1.0f - alpha) * tab_Box_x[Ipd] + alpha * tab_Box_x[Ipa];

        boxObject->objectTranslation[1] = (1.0f - alpha) * tab_Box_y[Ipd] + alpha * tab_Box_y[Ipa];

        boxObject->objectTranslation[2] = (1.0f - alpha) * tab_Box_z[Ipd] + alpha * tab_Box_z[Ipa];

        boxObject->objectTranslation[0] = 0;

        boxObject->objectTranslation[1] = 10;

        boxObject->objectTranslation[2] = 0;

    }
}

void box_repet_geste( int geste[], int taille, double duree, double t0, int n ){

    double duree_1 = duree/n;

    for( int i = 0; i < n; i ++) {
        float T0_i = t0 + duree_1 * i;
        box_interp_geste( geste, taille, duree_1, T0_i );
    }

}*/


void box_animation(float T0, float delay){

    float t = RealTime() - InitialTime - T0 - delay;

    double theta;

    if( t >= 0){

        theta = 5*sin(t*2);

        Ym *= 0.99;

        Zm *= 0.99;

        /*boxe_vit_y = Ym * sin(theta*PI/180);
        boxe_vit_z = Zm * sin(theta*PI/180) * sin(theta*PI/180);*/

        boxe_vit_y = Ym * (theta*PI/180);
        boxe_vit_z = Zm * (theta*PI/180) * (theta*PI/180);

        //cout << t << endl;
        //cout << (theta*PI/180) << endl;

        boxObject->objectTranslation[0] = boxe_vit_x;

        if( bodyObject->direction == 'l' ){
            boxObject->objectTranslation[1] = -2 - boxe_vit_y;

        } else if( bodyObject->direction == 'r' ){
            boxObject->objectTranslation[1] = -2 + boxe_vit_y;
        }

        boxObject->objectTranslation[2] = 4.5 + boxe_vit_z;

        if( t > 6.5 ){
            Ym = 30;
            Zm = 30;
            boxe_droite = false;
            boxe_gauche = false;
        }
    }

}

bool collisionDetection(){
    if( bodyObject->direction == 'l'){
        if( bodyObject->objectTranslation[1] <=  boxObject->objectTranslation[1] + 2.3 &&
            bodyObject->objectTranslation[1] >  boxObject->objectTranslation[1]){
            return true;
        }
    } else if( bodyObject->direction == 'r' ){
        if( bodyObject->objectTranslation[1] >=  boxObject->objectTranslation[1] - 2.3 &&
            bodyObject->objectTranslation[1] <  boxObject->objectTranslation[1]){
            return true;
        }
    }
        return false;

}
//////////////////////////////////////////////////////////////////
  // keypoint interpolation
//////////////////////////////////////////////////////////////////
void KF_Interpolation( void ) {


  double t = RealTime() - InitialTime;

  // 1 pose per second
  // KEYFRAME INDEX IS THE INTEGER NUMBER OF SECONDS
  // current frame
  int indKF = (int)floor( t ) % 10;
  // next frame
  int indNextKF = (indKF + 1) % 10;



  // INTERPOLATION VALUE IS THE DECIMAL OF SECONDS
  float alpha = t - floor( (int)t );

  // printf("KF#%d-%d alpha %.2f\n",indKF,indNextKF,alpha);

  // assigns the translation to CurrentActiveKeyPoint
  /*for( int ind = 0 ; ind < bodyObject->NbKPs ; ind++ ) {
    bodyObject->TabKPs[ind].translation.x = (1.0f - alpha) * tab_KF_x[ind][indKF]
      + alpha * tab_KF_x[ind][indNextKF];
    bodyObject->TabKPs[ind].translation.y = (1.0f - alpha) * tab_KF_y[ind][indKF]
      + alpha * tab_KF_y[ind][indNextKF];
    bodyObject->TabKPs[ind].translation.z = (1.0f - alpha) * tab_KF_z[ind][indKF]
      + alpha * tab_KF_z[ind][indNextKF];
  }
  */
  // assigns the translation to CurrentActiveKeyPoint
  for( int ind = 0 ; ind < 8 ; ind++ ) {
    int boneIndex = tab_Bones_indices[ind];

    /////////////////////////////// TODO ///////////////////////////
    // transforms into quaternions the indKF and indNextKF
    // angle/axis values of the 4 posed bones
    glm::quat quat1; // quaternion of indKF pose
    glm::quat quat2; // quaternion of indNextKF pose

    quat1 = glm::angleAxis(
        (float) (tab_Bones_KF_a[ind][indKF] * M_PI / 180.0F),
        glm::normalize(glm::vec3(
            tab_Bones_KF_x[ind][indKF],
            tab_Bones_KF_y[ind][indKF],
            tab_Bones_KF_z[ind][indKF])));

    quat2 = glm::angleAxis(
        (float) (tab_Bones_KF_a[ind][indNextKF] * M_PI / 180.0F),
        glm::normalize(glm::vec3(
            tab_Bones_KF_x[ind][indNextKF],
            tab_Bones_KF_y[ind][indNextKF],
            tab_Bones_KF_z[ind][indNextKF])));

    glm::quat quatInterp = glm::mix(quat1, quat2, alpha);
    bodyObject->TabBones[boneIndex].boneAnimationRotationMatrix =
        glm::mat4_cast(quatInterp);

    /////////////////////////////// TODO ///////////////////////////
    // assigns to boneAnimationRotationMatrix of bone no boneIndex
    // the mix of quat1 and quat2 by coeficient alpha
  }
}

//////////////////////////////////////////////////////////////////
// UPDATE FUNCTION
//////////////////////////////////////////////////////////////////
void updates( void ) {
  // process incoming messages from the interfaces
  processUDPMessages();

  //double t = RealTime() - InitialTime;
  // keypoint interpolation*
  // KF_Interpolation();
//int gestion[4] = {  8, 9, 10, 14   };          // 0, 1, 2, 3, 4      // 5, 6, 7, 8 , 9            // 10, 11, 12, 13, 14
  //KF_interp_geste(gestion, 4, 10, 0);
//KF_repet_geste(gestion, 4, 100, 5, 20);                                   // int geste[], taille de geste, temps total, temps init, fois

    //Initialisation des coordonnées de la sac de frappe
    //明天加上这一段话 初始化拳击袋的初始位置
    boxObject->objectTranslation[0] = 0;

    boxObject->objectTranslation[1] = -2.3;

    boxObject->objectTranslation[2] = 4.7;

   //marche
   if( marche == true ){
        translation_marche();
   }

  // ready
  if( ready == true ){
    //KF_Interpolation();
    translation_ready( t_ready );
  }
  // faire la boxe gauche
  if( boxe_gauche == true ){
    translation_boxe_gauche( t_boxe_gauche );
    if( collisionDetection() ){
        box_animation( t_boxe_gauche, 2);          // delay: 2
    }

  }
  // faire la boxe droite
  if( boxe_droite == true ){
    translation_boxe_droite( t_boxe_droite );
    if( collisionDetection() ){
        box_animation( t_boxe_droite, 2 );
    }

  }
  // changes the position of the vertices according to animation
  bodyObject->animate_points_in_mesh();

  // 1. copy back to the point buffer
  bodyObject->copy_mesh_points();

  // 2. update the buffer for the graphic board
  bodyObject->update_buffer_points();
}

//////////////////////////////////////////////////////////////////
// SKELETON ANIMATION
//////////////////////////////////////////////////////////////////

void render_one_bone( Bone *bone , glm::mat4 parentModelMatrix ) {
  if( !bone )
    return;

  /////////////////////////////// TODO ///////////////////////////
  // OpenGL transformations for drawing (structure)


  // computes the initial joint transformation matrices from
  // the initial rotation and translation matrices (should be
  // made once and for all in the future)
  if( bone->parentBone ) {
    /////////////////////////////// TODO ///////////////////////////
    // initial joint transformation: the initial tranformation
    // of the parents composed with local initial tranformations
    // bone->initialJointTransformation = ????;
    bone->initialJointTransformation =
        bone->parentBone->initialJointTransformation
        * bone->boneInitialTranslationMatrix
        * bone->boneInitialRotationMatrix;

    /////////////////////////////// TODO ///////////////////////////
    // current joint transformation: current joint transformation
    // of the parents combined with local initial transfomration and
    // animation rotation matrix
    // bone->currentJointTransformation = ???;
    bone->currentJointTransformation =
        bone->parentBone->currentJointTransformation
        * bone->boneInitialTranslationMatrix
        * bone->boneInitialRotationMatrix
        * bone->boneAnimationRotationMatrix;
  }
  else {
    /////////////////////////////// TODO ///////////////////////////
    // local initial joint transformation
    // bone->initialJointTransformation = ????;
    bone->initialJointTransformation =
        bone->boneInitialTranslationMatrix
        * bone->boneInitialRotationMatrix;

    /////////////////////////////// TODO ///////////////////////////
    // current joint transformation: local initial transfomration and
    // animation rotation matrix
    // bone->currentJointTransformation = ???;
    bone->currentJointTransformation =
        bone->boneInitialTranslationMatrix
        * bone->boneInitialRotationMatrix
        * bone->boneAnimationRotationMatrix;
  }

  // the point coordinates are in the mesh local coordinate system
  // we need to compose the current joint transformation by the
  // inverse of the initial joint transformation (for each mesh)

  /////////////////////////////// TODO ///////////////////////////
  /// calculates the inverse of the initial joint transformation
  glm::mat4 relativeTransformation =
    bone->currentJointTransformation
    * glm::inverse(bone->initialJointTransformation);

  for( int ind = 0 ; ind < MAX_MESHES ; ind++ ) {
    /////////////////////////////// TODO ///////////////////////////
    // differential transformation from current transformation
    // to initial transformation for mesh skinning
    // combines the current joint transformation by the inverse
    // of the initial joint transformation
    // bone->pointAnimationMatrix[ ind ] = ????;
    bone->pointAnimationMatrix[ind] = relativeTransformation;

  }

  /////////////////////////////// TODO ///////////////////////////
  // OpenGL transformations for drawing (animation)
  // combination of local model matrix and bone animation matrix
  // localModelMatrix = ???;

  // local model matrix: combination of parent local matrix
  // with initial transformations (rotation and translation)
  glm::mat4 localModelMatrix = parentModelMatrix
    * bone->boneInitialTranslationMatrix
    * bone->boneInitialRotationMatrix
    * bone->boneAnimationRotationMatrix;

  // bone graphical rendering
  glUniformMatrix4fv(bodyShader->uniform_object_model, 1, GL_FALSE,
		     glm::value_ptr(localModelMatrix));
  glUniformMatrix4fv(bodyShader->uniform_object_inverseModel, 1, GL_FALSE,
		     glm::value_ptr(glm::inverse(localModelMatrix)));
  GLfloat boneColor[3] = {0.8,0.2,0.7};
  glUniform3fv(bodyShader->uniform_object_objectColor,
	       1 , boneColor);

  // draw triangle strips from the currently bound VAO
  // with current in-use shader
  glBindVertexArray (bone->vao);
  glDrawArrays(GL_TRIANGLES , 0 , 2 * 3 );

  // recursive call
  render_one_bone( bone->daughterBone , localModelMatrix );

  // recursive call
  render_one_bone( bone->sisterBone , parentModelMatrix );
}

void render_bones( glm::mat4 modelMatrix ) {
  // calls rendering on the root bone (the first in the table of bones)
  render_one_bone( bodyObject->TabBones , modelMatrix );
}

//////////////////////////////////////////////////////////////////
// DRAWING FUNCTION
//////////////////////////////////////////////////////////////////

void draw( GLFWwindow* window ) {

  NbFrames++;

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable (GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

  // MVP matrices
  glm::mat4 projectionPerspMatrix; // Store the perspective projection matrix
  glm::mat4 viewPerspMatrix; // Store the view matrix
  glm::mat4 modelPerspMatrix; // Store the model matrix
  glm::mat4 modelPerspMatrix_box; //Store the model matrix de box

  // projection matrix
  projectionPerspMatrix
    = glm::frustum(-0.512f , 0.512f , -0.384f , 0.384f , near, 200.0f);

  // Camera matrix
  viewPerspMatrix
    = glm::lookAt(
		  glm::vec3(DistanceFactor * eyePosition[0],
			    eyePosition[1],
			    eyePosition[2]), // Camera is at DistanceFactor*eyePosition
		  glm::vec3(0,0,eyePosition[2]), // and looks horizontally
		  glm::vec3(0,0,1)  // Head is up (set to 0,0,1)
		  );

  // Model matrix : a varying rotation matrix (around Oz)
  // applies mouse based rotations
  if( drag ) {
    double x = 0;
    double y = 0;
    glfwGetCursorPos (window, &x , &y);
    bodyObject->objectAngle_y += y - mouse_pos_y;
    bodyObject->objectAngle_z += x - mouse_pos_x;

    // applies mouse based rotations à box
    //boxObject->objectAngle_y += y - mouse_pos_y;
    //boxObject->objectAngle_z += x - mouse_pos_x;

    mouse_pos_x = x;
    mouse_pos_y = y;
    // printf("drag %.1f %.1f %.1f %.1f\n" , x,y,mouse_pos_x,mouse_pos_y);

  }

  // translation of the object
  glm::vec3 myTranslation( bodyObject->objectTranslation[0] ,
			   bodyObject->objectTranslation[1] ,
			   bodyObject->objectTranslation[2] );

  // translation of the box
  glm::vec3 myTranslation_box( boxObject->objectTranslation[0] ,
			   boxObject->objectTranslation[1] ,
			   boxObject->objectTranslation[2] );


  // rotation of the object
  glm::vec3 myRotationZAxis( 0.0f , 0.0f , 1.0f );
  glm::vec3 myRotationYAxis( 0.0f , 1.0f , 0.0f );

  // modele matrix calculation
  modelPerspMatrix
    = glm::translate( glm::mat4(1.0f), myTranslation )  //ajouter
    * glm::rotate( glm::mat4(1.0f),
		   (float)bodyObject->objectAngle_z/100.f ,
		   myRotationZAxis )
    * glm::rotate( glm::mat4(1.0f),
		   (float)bodyObject->objectAngle_y/100.f ,
		   myRotationYAxis );

  modelPerspMatrix_box
    = glm::translate( glm::mat4(1.0f),  myTranslation_box )  //ajouter
    * glm::rotate( glm::mat4(1.0f),
		   (float)boxObject->objectAngle_x/100.f ,
		   myRotationZAxis )
    * glm::rotate( glm::mat4(1.0f),
		   (float)boxObject->objectAngle_z/100.f ,
		   myRotationZAxis )
    * glm::rotate( glm::mat4(1.0f),
		   (float)boxObject->objectAngle_y/100.f ,
		   myRotationYAxis );


  // output buffer cleanup
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ////////////////////////////////////////
  // activate shaders and sets uniform variable values
  glUseProgram (bodyShader->shader_programme);
  glBindVertexArray (bodyObject->vaoMesh);
  glUniformMatrix4fv(bodyShader->uniform_object_proj, 1, GL_FALSE,
		     glm::value_ptr(projectionPerspMatrix));
  glUniformMatrix4fv(bodyShader->uniform_object_view, 1, GL_FALSE,
		     glm::value_ptr(viewPerspMatrix));
  glUniformMatrix4fv(bodyShader->uniform_object_model, 1, GL_FALSE,
		     glm::value_ptr(modelPerspMatrix));
  glUniformMatrix4fv(bodyShader->uniform_object_inverseModel, 1, GL_FALSE,
		     glm::value_ptr(glm::inverse(modelPerspMatrix)));

  glUniform3fv(bodyShader->uniform_object_light, 1, lightPosition);
  glUniform3f(bodyShader->uniform_object_eye,
	      DistanceFactor * eyePosition[0],
	      eyePosition[1],
	      eyePosition[2]);
  glUniform3fv(bodyShader->uniform_object_objectColor,
	       1 , bodyObject->objectColor);

  // draw triangle strips from the currently bound VAO
  // with current in-use shader
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, bodyObject->vboMeshIndex);
  glDrawElements(GL_TRIANGLES, bodyObject->NbFaces * 3 ,
		 GL_UNSIGNED_INT, (GLvoid*)0);


  //Box
  glBindVertexArray (boxObject->vaoMesh);
  glUniformMatrix4fv(bodyShader->uniform_object_proj, 1, GL_FALSE,
		     glm::value_ptr(projectionPerspMatrix));
  glUniformMatrix4fv(bodyShader->uniform_object_view, 1, GL_FALSE,
		     glm::value_ptr(viewPerspMatrix));
  glUniformMatrix4fv(bodyShader->uniform_object_model, 1, GL_FALSE,
		     glm::value_ptr(modelPerspMatrix_box));
  glUniformMatrix4fv(bodyShader->uniform_object_inverseModel, 1, GL_FALSE,
		     glm::value_ptr(glm::inverse(modelPerspMatrix_box)));

  glUniform3fv(bodyShader->uniform_object_objectColor,
	       1 , boxObject->objectColor);

  // draw triangle strips from the currently bound VAO of box with current in-use shader
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, boxObject->vboMeshIndex);
  glDrawElements(GL_TRIANGLES, boxObject->NbFaces * 3 ,
		 GL_UNSIGNED_INT, (GLvoid*)0);


  // draws bones and updates the model matrix for each bone
  // according to its current transformation
  glDisable (GL_DEPTH_TEST); // enable depth-testing
  render_bones( modelPerspMatrix );


  glBindVertexArray(0); // Disable our Vertex Buffer Object
}

//////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  ///////////////////////////////////////////////////////////
  // initial time
  InitialTime = RealTime();
  //printf("%lf\n", RealTime());
  ///////////////////////////////////////////////////////////
  // mesh intialization
  bodyObject = new Object;
  bodyObject->init_mesh();

  boxObject = new Object;
  boxObject->init_mesh();

  ///////////////////////////////////////////////////////////
  // parses the mesh (obj format)
  if( argc >=2 ) {
    strcpy( bodyObject->MeshFileName , argv[1] );
    strcpy( boxObject->MeshFileName , argv[1] );
  }
  else {
    printf( "Mesh file (a.obj): " );
    fflush( stdin );
    fgets( bodyObject->MeshFileName , STRINGSIZE , stdin );

    printf( "Mesh file (boxe.obj): " );
    fflush( stdin );
    fgets( boxObject->MeshFileName , STRINGSIZE , stdin );

    if( *(bodyObject->MeshFileName) == '\n' ) {
      strcpy( bodyObject->MeshFileName , "a.obj" ); ;
    }
    else {
      bodyObject->MeshFileName[ strlen( bodyObject->MeshFileName ) - 1 ] = 0;
    }

    if( *(boxObject->MeshFileName) == '\n' ) {
      strcpy( boxObject->MeshFileName , "boxe.obj" ); ;
    }
    else {
      boxObject->MeshFileName[ strlen( boxObject->MeshFileName ) - 1 ] = 0;
    }
  }

  strcpy( bodyObject->KPFileName , bodyObject->MeshFileName ); ;
  bodyObject->KPFileName[ strlen( bodyObject->KPFileName ) - 4 ] = 0;
  strcat( bodyObject->KPFileName , "_KP.obj" );

  strcpy( boxObject->KPFileName , boxObject->MeshFileName ); ;
  boxObject->KPFileName[ strlen( boxObject->KPFileName ) - 4 ] = 0;
  strcat( boxObject->KPFileName , "_KP.obj" );

  printf( "Mesh file (%s)\n" , bodyObject->MeshFileName );
  printf( "KP file (%s)\n" , bodyObject->KPFileName );

  FILE * fileMesh = fopen( bodyObject->MeshFileName , "r" );
  if( !fileMesh ) {
    printf( "File %s not found\n" , bodyObject->MeshFileName );
    exit(0);
  }
  bodyObject->parse_mesh( fileMesh );
  fclose( fileMesh );
  // ajouter le file de BOX
  FILE * fileMesh_box = fopen( boxObject->MeshFileName , "r" );
  if( !fileMesh_box ) {
    printf( "File %s not found\n" , boxObject->MeshFileName );
    exit(0);
  }
  boxObject->parse_mesh( fileMesh_box );
  fclose( fileMesh_box );

  // checks that the bones in the animation table are found inside the mesh
  for(int ind = 0 ; ind < 8 ; ind++ ) {
    tab_Bones_indices[ind] = bodyObject->BoneIndex( tab_Bones_ID[ind] );
  }

  // initially no animation
  for( int index = 0 ; index < 3 * bodyObject->NbFaces ; index++) {
    bodyObject->TabPoints[index].cur_x = bodyObject->TabPoints[index].x;
    bodyObject->TabPoints[index].cur_y = bodyObject->TabPoints[index].y;
    bodyObject->TabPoints[index].cur_z = bodyObject->TabPoints[index].z;
  }

  // initially no animation of BOX
  for( int index = 0 ; index < 3 * boxObject->NbFaces ; index++) {
    boxObject->TabPoints[index].cur_x = boxObject->TabPoints[index].x;
    boxObject->TabPoints[index].cur_y = boxObject->TabPoints[index].y;
    boxObject->TabPoints[index].cur_z = boxObject->TabPoints[index].z;
  }

  ///////////////////////////////////////////////////////////
  // mesh copy into the buffers
  bodyObject->copy_mesh_points();
  bodyObject->copy_mesh_normals();
  bodyObject->copy_mesh_faces();

  //box mesh copy into the buffers
  boxObject->copy_mesh_points();
  boxObject->copy_mesh_normals();
  boxObject->copy_mesh_faces();

  ///////////////////////////////////////////////////////////
  // parses the keypoints
  FILE * fileKP = fopen( bodyObject->KPFileName , "r" );
  if( !fileKP ) {
    printf( "File %s not found, no keypoint defined for this mesh\n" ,
	    bodyObject->KPFileName );
  }
  else {
    bodyObject->parse_KP_obj( fileKP );
    fclose( fileKP );
  }

  // parses the keypoints of box
  fileKP = fopen( boxObject->KPFileName , "r" );
  if( !fileKP ) {
    printf( "File %s not found, no keypoint defined for this mesh\n" ,
	    boxObject->KPFileName );
  }
  else {
    boxObject->parse_KP_obj( fileKP );
    fclose( fileKP );
  }

  // locates the keypoints in the mesh
  bodyObject->locate_KP_in_mesh();
  boxObject ->locate_KP_in_mesh();


  // weights the vertices on these keypoints
  bodyObject->weight_points_on_KP_in_mesh( 0.03 , 0 , &linear_weight );
  boxObject ->weight_points_on_KP_in_mesh( 0.03 , 0 , &linear_weight );


  ///////////////////////////////////////////////////////////
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit ()) {
    fprintf (stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  GLFWwindow* window
    = glfwCreateWindow (width, height, "OpenGL Frame", NULL, NULL);
  if (!window) {
    fprintf (stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent (window);

  // Set key callback function
  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCharCallback(window, char_callback);
  glfwSetMouseButtonCallback(window, mouse_callback);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit ();

  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s\n", version);

  ///////////////////////////////////////////////////////////
  // vertex buffer objects and vertex array for the mesh
  bodyObject->init_geometry_buffers();
  boxObject ->init_geometry_buffers();

  bodyObject->objectColor[0]=0.8;                                                           // Changer couleurs  valeur : 0-1
  bodyObject->objectColor[1]=0.2;
  bodyObject->objectColor[2]=0.7;

  ///////////////////////////////////////////////////////////
  // shader intialization
  bodyShader = new Shader;
  bodyShader->init_shader( (char *)"src/Mesh-display-VS.glsl" ,
			   (char *)"src/Mesh-display-FS.glsl" );

  ///////////////////////////////////////////////////////////
  // keypoint listing
  printf( "Keypoints " );
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    printf( "%s " , tab_ID[ind] );
  }
  printf( "\n" );

  ///////////////////////////////////////////////////////////
  // UDP server initialization
  initUDP();

  ////////////////////////////////////////
  // endless rendering loop

  // the time of the previous frame
  while (!glfwWindowShouldClose (window)) {
    // draws
    draw(window); //l'affichage

    // updates
    updates();

    // update other events like input handling
    glfwPollEvents ();

    // put the stuff we've been drawing onto the display
    glfwSwapBuffers (window);
  }


  ////////////////////////////////////////
  // close GL context and any other GLFW resources
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
