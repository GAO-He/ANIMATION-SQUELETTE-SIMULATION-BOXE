/*****************************************************************************
File: utils.h

Informatique Graphique
Master d'informatique
Christian Jacquemin, Universite Paris-Sud & LIMSI-CNRS

Copyright (C) 2016 University Paris-Sud
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#define GLM_FORCE_RADIANS
#define    STRINGSIZE 80

////////////////////////////////////////
// geometrical data of mesh
#define MAX_FACES  10000   // maximal number of faces (triangles)
#define MAX_MESHES 30      // maximal number of meshes (sets of triangles)

#define MAX_KPS    100     // maximal number of key points
#define MAX_BONES  200     // maximal number of bones

#define MAX_KPWEIGHTS  4   // maximal number of keypoint weights per vertex
#define MAX_BONEWEIGHTS  6

//#define WIN32
#ifdef WIN32
#include <sys/time.h>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <glm.hpp>
#include <ext.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
using namespace std ;
#include <fstream>
#include <sys/time.h>
#else
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
using namespace std ;
#include <math.h>
#include <fstream>
#include <sys/time.h>
#endif

extern double InitialTime;
extern char tab_ID[4][16];
extern float tab_KF_x[4][10];
extern float tab_KF_y[4][10];
extern float tab_KF_z[4][10];

// function prototypes
bool loadtexture( string flieName , int index , bool isRect );
void loadgeometry( void );
void printLog(GLuint obj);
unsigned long getFileLength(ifstream& file);
int loadshader(string filename, GLuint shader);
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode,
			 int action, int mods);
void char_callback(GLFWwindow* window, unsigned int key);
void mouse_callback(GLFWwindow* window, unsigned int key);
double RealTime( void );

extern bool perspective;
extern GLuint textureID[];
extern cv::Mat image[];


//////////////////////////////////////////////////////////////////
// GESTION
//////////////////////////////////////////////////////////////////
/*class Geste {
public :
    int taille;
    int tab [100];

    Geste(void) {
        this->taille = 0;
    }

    Geste( int n ){
        taille = n;
    }

    ~Geste( void ) {

    }
};


class box{

    double vit_x = 0;
    double vit_y = 0;
    double vit_z = 0;

}
*/
////////////////////////////////////////
// ELEMENTARY GEOMETRICAL CLSSES: MESH DATA
////////////////////////////////////////
class Vector {
public:
  float x, y, z;
  Vector( void ) {
    init();
  };
  ~Vector( void ) {
  };
  void init( void ) {
    x = 0;
    y = 0;
    z = 0;
  };
  void normalize( void ) {
    if( x == 0 && y == 0 && z == 0 ) {
      return;
    }
    float norm = 1.0 / sqrt( x*x + y*y + z*z );
    x *= norm;
    y *= norm;
    z *= norm;
  }
  // 1 vector
  float prodScal( Vector &v2 ) {
    return x * v2.x + y * v2.y + z * v2.z;
  }
  // average
  void averageVectors( Vector *vectors , int nbVectors ) {
    x = 0; y = 0; z = 0;
    if( nbVectors <= 0 ) {
      return;
    }
    for( int ind = 0 ; ind < nbVectors ; ind++ ) {
      x += vectors[ ind ].x;
      y += vectors[ ind ].y;
      z += vectors[ ind ].z;
    }
    float inv = 1.0 / (float)nbVectors;
    x *= inv;
    y *= inv;
    z *= inv;
  }
  void operator*=(double d) {
    x *= d;
    y *= d;
    z *= d;
  }
  void operator/=(double d) {
    x /= d;
    y /= d;
    z /= d;
  }
  void operator+=(Vector& v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }
  int operator==(Vector& v) {
    return((x == v.x) && (y == v.y) && (z == v.z));
  }
  float norm(void) {
    return sqrt(x*x + y*y + z*z);
  }
  float norm2(void) {
    return (x*x + y*y + z*z);
  }
};

class Point {
public:
  float      x, y, z;
  float      cur_x, cur_y, cur_z;

  // 4 weights on keypoints (other vertices in the mesh)
  float      wKP[MAX_KPWEIGHTS];
  // 4 indices of the weighted keypoints (other vertices in the mesh)
  int        indKP[MAX_KPWEIGHTS];

  // 4 weights on bones
  float      wBones[MAX_BONEWEIGHTS];
  // 4 indices of the weighted bones
  int        indBones[MAX_BONEWEIGHTS];

  // boolean working variable to memorize weighting
  bool       weighted;
  bool       updated;

  Point( void ) {
    init();
  };
  ~Point( void ) {
  };
  void init( void ) {
    x = 0;
    y = 0;
    z = 0;
    cur_x = 0;
    cur_y = 0;
    cur_z = 0;
    for( int ind = 0 ; ind < MAX_KPWEIGHTS ; ind++ ) {
      wKP[ind] = 0.0;
      indKP[ind] = -1;
    }
    for( int ind = 0 ; ind < MAX_BONEWEIGHTS ; ind++ ) {
      wBones[ind] = 0.0;
      indBones[ind] = -1;
    }
    weighted = false;
    updated = false;
  };
  void operator=(Point& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    cur_x = v.cur_x;
    cur_y = v.cur_y;
    cur_z = v.cur_z;
    for( int ind = 0 ; ind < MAX_KPWEIGHTS ; ind++ ) {
      wKP[ind] = v.wKP[ind];
      indKP[ind] = v.indKP[ind];
    }
    for( int ind = 0 ; ind < MAX_BONEWEIGHTS ; ind++ ) {
      wBones[ind] =  v.wBones[ind];
      indBones[ind] = v.indBones[ind];
    }
    weighted = v.weighted;
    updated = v.updated;
  }
  void operator/=(double d) {
    x /= d;
    y /= d;
    z /= d;
  }
  void operator*=(double d) {
    x *= d;
    y *= d;
    z *= d;
  }
  void operator+=(Vector& v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }
  void operator*(double f) {
    x = f * x;
    y = f * y;
    z = f * z;
  }
  int operator==(Point& v) {
    return((x == v.x) && (y == v.y) && (z == v.z));
  }
  float distance(Point& p) {
    float dx, dy, dz;
    dx = p.x - x;
    dy = p.y - y;
    dz = p.z - z;
    return sqrt(dx*dx + dy*dy + dz*dz);
  }
  void product( Point &p , float * matrixValue ) {
    x = p.x * matrixValue[0] + p.y * matrixValue[4]
      + p.z * matrixValue[8] + matrixValue[12];
    y = p.x * matrixValue[1] + p.y * matrixValue[5]
      + p.z * matrixValue[9] + matrixValue[13];
    z = p.x * matrixValue[2] + p.y * matrixValue[6]
      + p.z * matrixValue[10] + matrixValue[14];
  }
};

class Face {
public:
  int index1;
  int index2;
  int index3;
  Face( void ) {
    index1 = -1;
    index2 = -1;
    index3 = -1;
  }
  ~Face( void ) {
  };
};

class Mesh {
public:
  char *id;
  char *matId;
  int indFaceIni;
  int indFaceEnd;
  int nbKPs;
  Mesh( void ) {
    id = new char[STRINGSIZE];
    id[0] = 0;
    matId = new char[STRINGSIZE];
    matId[0] = 0;
    indFaceIni = 0;
    indFaceEnd = 0;
    nbKPs = 0;
  }
  ~Mesh( void ) {
    delete [] id;
    delete [] matId;
  }
};

//////////////////////////////////////////////////////////////////
// KEYPOINTS
//////////////////////////////////////////////////////////////////

enum WeightingType{ NoWeighting = 0 , Weighting , WeightSubstitution };

class KP {
public:
  // keypoint ID (also reported in the vertex)
  char *id;
  // initial coordinates
  Point location;
  //  index of the mesh
  int indMesh;
  //  index of vertex
  int index;
  // current translation
  Vector translation;

  KP( void ) {
    id = new char[STRINGSIZE];
    location.init();
    indMesh = -1;
    index = -1;
    translation.init();
  }
};

//////////////////////////////////////////////////////////////////
// SKELETON
//////////////////////////////////////////////////////////////////

class Bone {
public:
  // bone ID
  char *id;
  // bone length (along y axis)
  float length;
  // parent bone if there's one
  Bone *daughterBone;
  Bone *parentBone;
  Bone *sisterBone;

  ////////////////////////////////////////
  // geometrical data
  float points[2 * 3 * 3];
  unsigned int vbo;
  unsigned int vao;

  // initial translation matrix computed from translation vector
  glm::mat4      boneInitialTranslationMatrix;

  // initial and animation rotation computed from axis and angle
  glm::mat4      boneAnimationRotationMatrix;
  glm::mat4      boneInitialRotationMatrix;

  // joint Transformation Matrices (initial and current)
  glm::mat4      initialJointTransformation;
  glm::mat4      currentJointTransformation;

  // vertex animation Matrix expressed in the mesh local coordinates
  glm::mat4      pointAnimationMatrix[MAX_MESHES];

  Bone( void ) {
    id = new char[STRINGSIZE];
    length = 0;
    daughterBone = NULL;
    parentBone = NULL;
    sisterBone = NULL;

    boneInitialTranslationMatrix = glm::mat4(1.0f);
    boneInitialRotationMatrix = glm::mat4(1.0f);
    boneAnimationRotationMatrix = glm::mat4(1.0f);

    for( int ind = 0 ; ind < MAX_MESHES ; ind++ ) {
      pointAnimationMatrix[ind] = glm::mat4(1.0f);
    }

    initialJointTransformation = glm::mat4(1.0f);
    currentJointTransformation = glm::mat4(1.0f);

    // 1st triangle
    points[0] = 0.02;
    points[1] = 0.0;
    points[2] = 0.02;
    points[3] = -0.02;
    points[4] = 0.0;
    points[5] = -0.02;
    points[6] = 0.0;
    points[7] = 1.0;
    points[8] = 0.0;
    // 2nd triangle
    points[9]  = -0.02;
    points[10] = 0.0;
    points[11] = 0.02;
    points[12] = 0.02;
    points[13] = 0.0;
    points[14] = -0.02;
    points[15] = 0.0;
    points[16] = 1.0;
    points[17] = 0.0;

  }
  ~Bone( void ) {
    delete [] id;
  }
};
//////////////////////////////////////////////////////////////////
// OBJECT: A SET OF MESHES
//////////////////////////////////////////////////////////////////

class Object {
  Point    *TabPointsIni; // temporary normal storage for index
  Vector   *TabNormalsIni; // temporary normal storage for index
 public:
  Point    *TabPoints;    // vertices
  Vector   *TabNormals;   // normals
  Face     *TabFaces;     // faces (triangles)
  Mesh     *TabMeshes;    // meshes (sets of triangles)

  ////////////////////////////////////////
  // keypoints
  KP       *TabKPs;       // key points

  int      NbPoints;      // number of vertices
  int      NbNormals;     // number of normals
  int      NbFaces;       // number of faces (triangles)
  int      NbMeshes;      // number of meshes (sets of triangles)

  int      NbKPs;         // number of key points
  int      CurrentActiveKeyPoint;
  char   direction;     // the direction to which the person face

  bool     meshChanged;   // mesh deformation through animation

  //////////////////////////////////////////////////////////////////
  // SKELETON DATA
  char     BoneFileName[STRINGSIZE];
  Bone     *TabBones = NULL;
  int      NbBones;
  int      CurrentActiveBone;

  ////////////////////////////////////////
  // geometrical data of mesh
  GLfloat *pointBuffer = NULL;
  GLfloat *normalBuffer = NULL;
  GLuint  *indexBuffer = NULL;

  // vertex array objects and vertex buffer objects
  unsigned int vboMeshPoints;
  unsigned int vboMeshNormals;
  unsigned int vboMeshIndex;
  unsigned int vaoMesh;

  // translation of the mesh
  float objectTranslation[3];
  double objectAngle_z, objectAngle_y, objectAngle_x;

  // translation of the mesh
  float objectRotation[3];

  // color
  GLfloat objectColor[3];

  // mesh file name
  char MeshFileName[STRINGSIZE];
  char MaterialFileName[STRINGSIZE];
  char KPFileName[STRINGSIZE];

  ////////////////////////////////////////
  // MESH DATA ALLOCATION
  ////////////////////////////////////////

  void init_mesh( void )
  {
    NbMeshes = 0;
    NbPoints = 0;
    NbFaces = 0;
    NbNormals = 0;
    direction = 'l';

    TabPointsIni = new Point[ MAX_FACES * 3 ];
    TabPoints = new Point[ MAX_FACES * 3 ];

    TabNormalsIni = new Vector[ MAX_FACES * 3 ];
    TabNormals = new Vector[ MAX_FACES * 3 ];

    TabFaces = new Face[ MAX_FACES ];
    TabMeshes = new Mesh[ MAX_MESHES ];

    TabKPs = new KP[ MAX_KPS ];

    TabBones = new Bone[ MAX_BONES ];

    NbPoints = 0;
    NbNormals = 0;
    NbFaces = 0;
    NbMeshes = 0;

    NbKPs = 0;
    CurrentActiveKeyPoint = 0;

    NbBones = 0;
    CurrentActiveBone = 0;

    // point positions and normals
    pointBuffer = (GLfloat *)malloc( MAX_FACES * 3 * 3
				     * sizeof *pointBuffer);
    normalBuffer = (GLfloat *)malloc( MAX_FACES * 3 * 3
				      * sizeof *normalBuffer);
    indexBuffer = (GLuint *)malloc( MAX_FACES * 3
				    * sizeof *indexBuffer);

    // white color
     //objectColor[0] = 0.9f;
    objectColor[0] = 0.0f;
    objectColor[1] = 0.9f;
    objectColor[2] = 0.9f;
  }

  //////////////////////////////////////////////////////////////////
  // MESH FILE PARSING
  //////////////////////////////////////////////////////////////////

  // OBJ file parsing (Alias Wavefront ASCII format)
  void parse_mesh( FILE *file )
  {
    char    tag[512];
    char    line[512];

    // Two comment lines
    // # Blender3D v244 OBJ File: Anime_Girl.blend
    // # www.blender3d.org
    if( !fgets  ( line, 512, file ) ) { return; }
    if( !fgets  ( line, 512, file ) ) { return; }

    // material name
    if( !fgets  ( line, 512, file ) ) { return; }
    sscanf ( line, "%s %s",
	     tag, MaterialFileName );

    // bone file name
    fgets  ( line, 512, file );
    sscanf ( line, "%s %s",
	     tag,
	     BoneFileName );
    printf( "Bone file(%s)\n" , BoneFileName );

    // parses the bones
    FILE * fileBone = fopen( BoneFileName , "r" );
    if( !fileBone ) {
      printf( "File %s not found, no bones defined for this mesh\n" , BoneFileName );
    }
    else {
      parse_Bone_obj( fileBone );
      fclose( fileBone );
    }

    // mesh ID
    if( !fgets  ( line, 512, file ) ) { return; }
    sscanf ( line, "%s", tag );

    while( strcmp( tag , "o" ) == 0 ) {
      if( NbMeshes > MAX_MESHES ) {
	printf( "Error: Excessive number of Meshes\n" );
	return;
      }

      // mesh ID
      sscanf ( line, "%s %s",
	       tag , TabMeshes[ NbMeshes ].id );

      // next tag
      if( !fgets  ( line, 512, file ) ) { return; }
      sscanf ( line, "%s", tag );

      // Scan for Points in this mesh
      int indPointIni = NbPoints;
      while( strcmp( tag , "v" ) == 0 ) {
	if( NbPoints > MAX_FACES * 3 ) {
	  printf( "Error: Excessive number of points\n" );
	  throw 0;
	}
	sscanf ( line, "%s %f %f %f",
		 tag,
		 &(TabPointsIni[NbPoints].x),
		 &(TabPointsIni[NbPoints].y),
		 &(TabPointsIni[NbPoints].z) );
	NbPoints++;

	if( !fgets  ( line, 512, file ) ) { return; }
	sscanf ( line, "%s", tag );
      }

      // Scan for Parent Bones in this mesh
      int nbBonesLoc = 0;
      int TabBonesLoc[MAX_BONES];
      while( strcmp( tag , "bone" ) == 0 ) {
	if( NbBones > 0 ) {
	  if( nbBonesLoc >= MAX_BONES ) {
	    printf( "Error: Excessive number of bones in object %s\n" ,
		    TabMeshes[ NbMeshes ].id  );
	    throw 0;
	  }

	  char    boneID[512];
	  sscanf ( line, "%s %s", tag , boneID );
	  bool bonefound = false;
	  for( int indAux = 0 ; indAux < NbBones ; indAux++ ) {
	    if( strcmp( TabBones[indAux].id , boneID ) == 0 ) {
	      bonefound = true;
	      TabBonesLoc[nbBonesLoc] = indAux;
	      break;
	    }
	  }
	  if( !bonefound ) {
	    printf( "Non bone parent group %s in object %s\n" , boneID ,
		    TabMeshes[ NbMeshes ].id );
	  }
	}

	if( !fgets  ( line, 512, file ) ) { return; }
	sscanf ( line, "%s", tag );
	nbBonesLoc++;
      }

      // scans the weight vectors
      while( strcmp( tag , "vw" ) == 0 ) {
	int indPoint;
	int indBone;
	float w;

	sscanf ( line, "%s %d %d %f", tag , &indPoint , &indBone , &w );
	indPoint -= 1;
	indPoint += indPointIni;
	indBone -= 1;

	// Scan for Bones in this mesh
	if( indBone >= NbBones ) {
	  printf( "Error: Incorrect bone index\nLine: %sBone index %d, Nb Bones %d, %dth weight for point %d\n" ,
		  line, indBone , NbBones , nbBonesLoc + 2 , indPoint - indPointIni + 1 );
	  throw 0;
	}

	bool weightassigned = false;
	for( int indWeight = 0 ; indWeight < MAX_BONEWEIGHTS ; indWeight++ ) {
	  if( TabPointsIni[indPoint].wBones[indWeight] <= 0.0 ) {
	    TabPointsIni[indPoint].indBones[indWeight]
	      = TabBonesLoc[indBone];
	    TabPointsIni[indPoint].wBones[indWeight] = w;
	    // printf( "point %d %s %f\n" , indPoint - indPointIni + 1,
	    // 	  TabBones[TabPoints[indPoint].indBones[indWeight]].id,
	    //   TabPoints[indPoint].wBones[indWeight] );
	    weightassigned = true;
	    break;
	  }
	}

	if( !weightassigned ) {
	  printf( "Error: Excessive number of bone weigths in object %s\n" ,
		  TabMeshes[ NbMeshes ].id  );
	  throw 0;
	}

	if( !fgets  ( line, 512, file ) ) { return; }
	sscanf ( line, "%s", tag );
      }

      // Scan for UV texture coordinates in this mesh (not used currently)
      while( strcmp( tag , "vt" ) == 0 ) {
	if( !fgets  ( line, 512, file ) ) { return; }
	sscanf ( line, "%s", tag );
      }

      // Scan for Normals in this mesh
      while( strcmp( tag , "vn" ) == 0 ) {
	if( NbNormals > MAX_FACES * 3 ) {
	  printf( "Error: Excessive number of normals\n" );
	  throw 0;
	}
	sscanf ( line, "%s %f %f %f",
		 tag ,
		 &(TabNormalsIni[NbNormals].x),
		 &(TabNormalsIni[NbNormals].y),
		 &(TabNormalsIni[NbNormals].z) );
	NbNormals++;

	if( !fgets  ( line, 512, file ) ) { return; }
	sscanf ( line, "%s", tag );
      }

      // Scan for Mat in this mesh
      if( strcmp( tag , "usemtl" ) == 0 ) {
	sscanf ( line, "%s %s",
		 tag , TabMeshes[ NbMeshes ].matId );
	if( !fgets  ( line, 512, file ) ) { return; }
	sscanf ( line, "%s", tag );
      }

      TabMeshes[NbMeshes].indFaceIni = NbFaces;

      // Scan for Faces in this mesh
      while( strcmp( tag , "f" ) == 0
	     || strcmp( tag , "usemtl" ) == 0
	     || strcmp( tag , "s" ) == 0 ) {
	if( NbFaces > MAX_FACES ) {
	  printf( "Error: Excessive number of faces\n" );
	  throw 0;
	}

	// Scan for Mat in this mesh
	// currently only one mat per mesh
	if( strcmp( tag , "usemtl" ) == 0 ) {
	  sscanf ( line, "%s", TabMeshes[ NbMeshes ].matId );
	}
	// Scan for Smooth boolean in this mesh
	else if( strcmp( tag , "s" ) == 0 ) {
	  sscanf ( line, "%s", tag );
	}
	// Scan for a Face in this mesh
	else {
	  int indPoint1;
	  int indPoint2;
	  int indPoint3;
	  int indNormal1;
	  int indNormal2;
	  int indNormal3;
	  sscanf( line, "%s %d//%d %d//%d %d//%d",
		  tag,
		  &indPoint1, &indNormal1, &indPoint2, &indNormal2,
		  &indPoint3, &indNormal3 );

	  if( indPoint1 > 0 && indPoint2 > 0 && indPoint3 > 0
	      && indNormal1 > 0 && indNormal2 > 0 && indNormal3 > 0 ) {
	    // copies the normals and the points from the initial buffer to the final one
	    // so that points and normals have the same indices

	    // indices start from 1 in OBJ format
	    // we make start from 0 for C++

	    TabFaces[NbFaces].index1 = 3 * NbFaces;
	    TabFaces[NbFaces].index2 = 3 * NbFaces + 1;
	    TabFaces[NbFaces].index3 = 3 * NbFaces + 2;

	    TabPoints[ TabFaces[NbFaces].index1 ]
	      = TabPointsIni[ indPoint1 - 1 ];
	    TabPoints[ TabFaces[NbFaces].index2 ]
	      = TabPointsIni[ indPoint2 - 1 ];
	    TabPoints[ TabFaces[NbFaces].index3 ]
	      = TabPointsIni[ indPoint3 - 1 ];

	    TabNormals[ TabFaces[NbFaces].index1 ]
	      = TabNormalsIni[ indNormal1 - 1 ];
	    TabNormals[ TabFaces[NbFaces].index2 ]
	      = TabNormalsIni[ indNormal2 - 1 ];
	    TabNormals[ TabFaces[NbFaces].index3 ]
	      = TabNormalsIni[ indNormal3 - 1 ];

	    NbFaces++;
	  }
	}

	if( !fgets  ( line, 512, file ) ) {
	  TabMeshes[NbMeshes].indFaceEnd = NbFaces;
	  printf( "Mesh #%d %s Faces %d-%d\n" , NbMeshes ,
		  TabMeshes[ NbMeshes ].id ,
		  TabMeshes[ NbMeshes ].indFaceIni ,
		  TabMeshes[ NbMeshes ].indFaceEnd );
	  NbMeshes++;
	  return;
	}
	sscanf ( line, "%s", tag );
      }

      TabMeshes[NbMeshes].indFaceEnd = NbFaces;
      printf( "Mesh #%d %s Faces %d-%d\n" , NbMeshes ,
	      TabMeshes[ NbMeshes ].id ,
	      TabMeshes[ NbMeshes ].indFaceIni ,
	      TabMeshes[ NbMeshes ].indFaceEnd );
      NbMeshes++;
    }

    ///////////////////////////////////////////////////////////
    // mesh copy into the buffers
    copy_mesh_points();
    copy_mesh_normals();
    copy_mesh_faces();
  }

  ///////////////////////////////////////////////////////////
  // key-point OBJ file parsing
  // (inhouse format inspired from the Alias Wavefront ASCII format)

  void parse_KP_obj( FILE *file )
  {
    char    tag[512];
    char    line[512];
    char    meshID[512];
    int     indMesh;

    // Two comment lines
    // # Anim_Girl Facial animation keypoints
    // #
    fgets  ( line, 512, file );
    fgets  ( line, 512, file );

    // mesh ID
    fgets  ( line, 512, file );
    sscanf ( line, "%s", tag );

    NbKPs = 0;
    while( strcmp( tag , "o" ) == 0 ) {
      // mesh ID
      sscanf ( line, "%s %s",
	       tag , meshID );

      // finds the index of the mesh associated with this KP
      indMesh = -1;
      for( int ind = 0 ; ind < NbMeshes ; ind++ ) {
	if( strcmp( meshID , TabMeshes[ ind ].id ) == 0 ) {
	  indMesh = ind;
	  // printf( "Mesh #%d ID %s\n" , ind , meshID );
	}
      }
      if( indMesh == -1 ) {
	printf( "Error: KeyPoint Mesh ID [%s] not found\n" , meshID );
      }

      // next tag
      fgets  ( line, 512, file );
      sscanf ( line, "%s", tag );

      // Scan for KPs in this mesh
      int numberMeshKPs = 0;
      while( strcmp( tag , "kp" ) == 0 ) {
	if( NbKPs > MAX_KPS ) {
	  printf( "Error: Excessive number of KeyPoints\n" );
	  throw 0;
	}

	sscanf ( line, "%s %s",
		 tag, TabKPs[NbKPs].id );

	// stores the index of the mesh associated with this keypoint
	TabKPs[NbKPs].indMesh = indMesh;

	fgets  ( line, 512, file );
	sscanf ( line, "%s %f %f %f",
		 tag,
		 &(TabKPs[NbKPs].location.x),
		 &(TabKPs[NbKPs].location.y),
		 &(TabKPs[NbKPs].location.z) );
	// printf( "vertex %f %f %f\n" , TabPoints[NbPoints].location.x,
	// 	      TabPoints[NbPoints].location.y,
	// 	      TabPoints[NbPoints].location.z );

	if( !fgets  ( line, 512, file ) ) {
	  numberMeshKPs++;
	  NbKPs++;
	  if( indMesh >= 0 ) {
	    TabMeshes[indMesh].nbKPs = numberMeshKPs;
	  }
	  printf( "Mesh #%d %s KPs %d\n" , indMesh ,
		  TabMeshes[ indMesh ].id ,
		  TabMeshes[ indMesh ].nbKPs );
	  return;
	}

	sscanf ( line, "%s", tag );
	numberMeshKPs++;
	NbKPs++;
      }

      if( indMesh >= 0 ) {
	TabMeshes[indMesh].nbKPs = numberMeshKPs;
      }

      printf( "Mesh #%d %s KPs %d\n" , indMesh ,
	      TabMeshes[ indMesh ].id ,
	      TabMeshes[ indMesh ].nbKPs );
    }
  }

  //////////////////////////////////////////////////////////////////
  // KEYPOINT BINDING AND POINT WEIGHTING
  //////////////////////////////////////////////////////////////////

  // for each keypoint, finds the nearest vertex in mesh
  // not really used, just a check

  void locate_KP_in_mesh( void ) {
    for( int indMesh = 0 ; indMesh < NbMeshes ; indMesh++ ) {
      for( int indKP = 0 ; indKP < NbKPs ; indKP++ ) {
	if( TabKPs[indKP].indMesh == indMesh ) {
	  // accesses the points from a mesh and its faces
	  float minDist = FLT_MAX;
	  int indexKP = -1;
	  for (int indFace = TabMeshes[ indMesh ].indFaceIni ;
	       indFace < TabMeshes[ indMesh ].indFaceEnd ;
	       indFace++) {
	    float d;
	    if( (d = TabKPs[indKP].location.distance(
		     TabPoints[ TabFaces[indFace].index1 ] ))
		< minDist ) {
	      indexKP = TabFaces[indFace].index1;
	      minDist = d;
	    }
	    if( (d = TabKPs[indKP].location.distance(
		     TabPoints[ TabFaces[indFace].index2 ]))
		< minDist ) {
	      indexKP = TabFaces[indFace].index2;
	      minDist = d;
	    }
	    if( (d = TabKPs[indKP].location.distance(
		     TabPoints[ TabFaces[indFace].index3 ]))
		< minDist ) {
	      indexKP = TabFaces[indFace].index3;
	      minDist = d;
	    }
	  }
	  TabKPs[indKP].index = indexKP;
	  /* printf( "KP %s Mesh %s %f %f %f \n    -> Point %d %f %f %f dist %f\n" ,  */
	  /* 	  TabKPs[indKP].id , */
	  /* 	  TabMeshes[ indMesh ].id , */
	  /* 	  TabKPs[indKP].location.x , */
	  /* 	  TabKPs[indKP].location.y , */
	  /* 	  TabKPs[indKP].location.z , */
	  /* 	  indexKP + 1 , */
	  /* 	  TabPoints[indexKP].x , */
	  /* 	  TabPoints[indexKP].y , */
	  /* 	  TabPoints[indexKP].z , */
	  /* 	  minDist ); */
	}
      }
    }
  }

  // points weighting: weights all the points in a mesh

  void weight_points_on_KP_in_mesh( float radius , int exponent ,
				    float (*pt2Function)(float,float,int) ) {
    for( int indMesh = 0 ; indMesh < NbMeshes ; indMesh++ ) {
      for( int indKP = 0 ; indKP < NbKPs ; indKP++ ) {
	if( TabKPs[indKP].indMesh == indMesh ) {
	  int nbWeightedPoints = 0;

	  // marks all the points as unprocessed for the current keypoint
	  for( int index = 0 ; index < 3 * NbFaces ; index++ ) {
	    TabPoints[ index ].weighted = false;
	  }

	  // accesses the points from a mesh and its faces
	  for (int indFace = TabMeshes[ indMesh ].indFaceIni ;
	       indFace < TabMeshes[ indMesh ].indFaceEnd ;
	       indFace++) {
	    if( weight_one_point( TabFaces[indFace].index1 , indKP ,
				  radius , exponent ,
				  pt2Function ) == Weighting ) {
	      nbWeightedPoints++;
	    }
	    if( weight_one_point( TabFaces[indFace].index2 , indKP ,
				  radius , exponent ,
				  pt2Function ) == Weighting ) {
	      nbWeightedPoints++;
	    }
	    if( weight_one_point( TabFaces[indFace].index3 , indKP ,
				  radius , exponent ,
				  pt2Function ) == Weighting ) {
	      nbWeightedPoints++;
	    }
	  }

	  printf( "KP %s Mesh %s Nb weighted points %d\n" ,
		  TabKPs[indKP].id ,
		  TabMeshes[ TabKPs[indKP].indMesh ].id ,
		  nbWeightedPoints );
	}
      }
    }
  }

  // points weighting: each vertex is weighted on at most
  // four keypoints
  // if there are more than four keypoints with non null
  // weight for this vertex, only the 4 KPs with the heighest
  // weights are taken into account

  WeightingType weight_one_point( int index , int indKP ,
				  float radius , int exponent ,
				  float (*pt2Function)(float,float,int) ) {
    if( !TabPoints[ index ].weighted ) {
      TabPoints[ index ].weighted = true;

      float d = TabPoints[ index ].distance(
					    TabKPs[indKP].location );
      float w = pt2Function( d , radius , exponent );

      if( w > 0 ) {
	float minWeight = FLT_MAX;
	int indKP_minWeight = -1;
	for( int i = 0 ; i < MAX_KPWEIGHTS ; i++ ) {
	  // non allocated weight
	  if( TabPoints[ index ].indKP[i] < 0 ) {
	    TabPoints[ index ].indKP[i] = indKP;
	    TabPoints[ index ].wKP[i] = w;
	    /* printf( "KP %s Mesh %s Point %d Weight %.6f index(ini) %d\n" ,  */
	    /* 	    TabKPs[indKP].id , */
	    /* 	    TabMeshes[ TabKPs[indKP].indMesh ].id , */
	    /* 	    index , w , i ); */
	    return Weighting;
	  }
	  else if( TabPoints[ index ].wKP[i]
		   < minWeight ) {
	    minWeight = TabPoints[ index ].wKP[i];
	    indKP_minWeight = i;
	  }
	}
	// all the weights are allocated
	// the lowest one is replaced
	if( minWeight < w ) {
	  TabPoints[ index ].indKP[indKP_minWeight] = indKP;
	  TabPoints[ index ].wKP[indKP_minWeight] = w;
	  /* printf( "KP %s Mesh %s Point %d Weight %.3f index(subst) %d\n" ,  */
	  /* 	  TabKPs[indKP].id , */
	  /* 	  TabMeshes[ TabKPs[indKP].indMesh ].id , */
	  /* 	  index , w ,	indKP_minWeight ); */
	  return WeightSubstitution;
	}
      }
    }
    return NoWeighting;
  }

  //////////////////////////////////////////////////////////////////
  // MESH ANIMATION
  //////////////////////////////////////////////////////////////////

  // moves each point according to the translation
  // of the keypoints to which this point is attached

  void animate_one_point( int indMesh , int indPoint , Point *ptPoint ) {
    int        indKP;
    int        indBone;

    ptPoint->cur_x = ptPoint->x;
    ptPoint->cur_y = ptPoint->y;
    ptPoint->cur_z = ptPoint->z;
    if( ptPoint->indKP[0] >= 0 ) {
      for( int i = 0 ; i < MAX_KPWEIGHTS ; i++ ) {
	// positive weight
	if( (indKP = ptPoint->indKP[i]) >= 0 ) {
	  Vector t = TabKPs[indKP].translation;
	  t *= ptPoint->wKP[i];
	  ptPoint->cur_x += t.x;
	  ptPoint->cur_y += t.y;
	  ptPoint->cur_z += t.z;
	  // 	printf( "KP %s Mesh %s Point %d Weight %.3f index(ini) %d\n" ,
	  // 		TabKPs[indKP].id ,
	  // 		TabMeshes[ TabKPs[indKP].indMesh ].id ,
	  // 		index , w , i );
	}
      }
    }
    if( ptPoint->indBones[0] >= 0 ) {
      glm::vec4 barycenter( 0.0f , 0.0f , 0.0f , 1.0f );
      for( int i = 0 ; i < MAX_BONEWEIGHTS ; i++ ) {
	///////////////////////////////
	 ///////////////////////////
	// transforms the cur_x, cur_y, cur_z positions of each
	// point by the pointAnimationMatrix of the bones on which it
	// is weighted, and calculates the weighted sum of all these
	// transformations
	    if( (indBone = ptPoint->indBones[i]) >= 0 ) {
	      glm::mat4 transformationAnimationBone =
	        TabBones[indBone].pointAnimationMatrix[indMesh];
	      barycenter += ptPoint->wBones[i]
	        * transformationAnimationBone
	        * glm::vec4(ptPoint->cur_x, ptPoint->cur_y, ptPoint->cur_z, 1);
	    }
      }
      ptPoint->cur_x = barycenter[0];
      ptPoint->cur_y = barycenter[1];
      ptPoint->cur_z = barycenter[2];
    }
  }

  void animate_points_in_mesh( void ) {
    Point    *ptPoint = TabPoints;

    // marks all the points as unprocessed for the current keypoint
    for( int index = 0 ; index < 3 * NbFaces ; index++ ) {
      TabPoints[ index ].updated = false;
    }

    // point update must be made mesh by mesh
    // because it depends on the mesh local coordinates
    for( int indMesh = 0 ; indMesh < NbMeshes ; indMesh++ ) {
      for (int i = TabMeshes[ indMesh ].indFaceIni ;
	   i < TabMeshes[ indMesh ].indFaceEnd ; i++) {
	int indPoint = TabFaces[i].index1;
	ptPoint = TabPoints + indPoint;
	if( ! ptPoint->updated ) {
	  animate_one_point( indMesh , indPoint , ptPoint );
	}
	indPoint = TabFaces[i].index2;
	ptPoint = TabPoints + indPoint;
	if( ! ptPoint->updated ) {
	  animate_one_point( indMesh , indPoint , ptPoint );
	}
	indPoint = TabFaces[i].index3;
	ptPoint = TabPoints + indPoint;
	if( ! ptPoint->updated ) {
	  animate_one_point( indMesh , indPoint , ptPoint );
	}
      }
    }
  }

  ////////////////////////////////////////////////////////
  // MESH COPY TO VERTEX, NORMAL AND FACE INDEX BUFFERS
  ////////////////////////////////////////////////////////
  void copy_mesh_points( void ) {
    // copies the new mesh points inside the point buffer
    for( int index = 0 ; index < 3 * NbFaces ; index++ ) {
      pointBuffer[ 3 * index ] = TabPoints[index].cur_x;
      pointBuffer[ 3 * index + 1 ] = TabPoints[index].cur_y;
      pointBuffer[ 3 * index + 2 ] = TabPoints[index].cur_z;
    }
  }
  void copy_mesh_normals( void ) {
    // copies the new mesh normals inside the normal buffer
    for( int index = 0 ; index < 3 * NbFaces ; index++ ) {
      normalBuffer[ 3 * index ] = TabNormals[index].x;
      normalBuffer[ 3 * index + 1 ] = TabNormals[index].y;
      normalBuffer[ 3 * index + 2 ] = TabNormals[index].z;
    }
  }
  void copy_mesh_faces( void ) {
    // copies the new mesh indices inside the index buffer
    for( int indFace = 0 ; indFace < NbFaces ; indFace++ ) {
      indexBuffer[ 3 * indFace ] = TabFaces[indFace].index1;
      indexBuffer[ 3 * indFace + 1 ] = TabFaces[indFace].index2;
      indexBuffer[ 3 * indFace + 2 ] = TabFaces[indFace].index3;
    }
  }

  ////////////////////////////////////////////////////////
  // VERTEX, NORMAL AND FACE INDEX BUFFER UPDATES
  ////////////////////////////////////////////////////////
  void update_buffer_points( void ) {
    glBindBuffer (GL_ARRAY_BUFFER, vboMeshPoints);
    glBufferData (GL_ARRAY_BUFFER,
		  3 * 3 * NbFaces * sizeof (GLfloat),
		  pointBuffer,
		  GL_DYNAMIC_DRAW);
  }
  void update_buffer_normals( void ) {
    glBindBuffer (GL_ARRAY_BUFFER, vboMeshNormals);
    glBufferData (GL_ARRAY_BUFFER,
		  3 * 3 * NbFaces * sizeof (GLfloat),
		  normalBuffer,
		  GL_DYNAMIC_DRAW);
  }
  void update_buffer_faces( void ) {
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, vboMeshIndex);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER,
		  3 * NbFaces * sizeof (GLuint),
		  indexBuffer,
		  GL_STATIC_DRAW);
  }

  ////////////////////////////////////////
  // VERTEX, NORMAL AND FACE INDEX BUFFER  INITIALIZATION
  ////////////////////////////////////////

  void init_geometry_buffers( void ) {
    objectTranslation[0] = 0.0f;
    objectTranslation[1] = 0.0f;
    objectTranslation[2] = 0.0f;
    objectAngle_z = 0;
    objectAngle_y = 0;
    objectAngle_x = 0;

    ///////////////////////////////////////////////////////////
    // vertex buffer objects and vertex array for the bones
    for( int ind = 0 ; ind < MAX_BONES ; ind++ ) {
      TabBones[ind].vbo = 0;
      glGenBuffers (1, &(TabBones[ind].vbo));

      TabBones[ind].vao = 0;
      glGenVertexArrays (1, &(TabBones[ind].vao));

      // vertex buffer objects and vertex array
      glBindBuffer (GL_ARRAY_BUFFER, TabBones[ind].vbo);
      glBufferData (GL_ARRAY_BUFFER,
		    2 * 3 * 3 * sizeof (float),
		    TabBones[ind].points,
		    GL_STATIC_DRAW);

      glBindVertexArray (TabBones[ind].vao);
      glBindBuffer (GL_ARRAY_BUFFER, TabBones[ind].vbo);
      glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
      glEnableVertexAttribArray (0);
    }

    ///////////////////////////////////////////////////////////
    // vertex buffer objects and vertex array for the mesh
    vboMeshPoints = 0;
    glGenBuffers (1, &vboMeshPoints);
    update_buffer_points();

    vboMeshNormals = 0;
    glGenBuffers (1, &vboMeshNormals);
    update_buffer_normals();

    vboMeshIndex = 0;
    glGenBuffers (1, &vboMeshIndex);
    update_buffer_faces();

    vaoMesh = 0;
    glGenVertexArrays (1, &vaoMesh);
    glBindVertexArray (vaoMesh);

    // vertex positions are at location 0
    glBindBuffer (GL_ARRAY_BUFFER, vboMeshPoints);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    glEnableVertexAttribArray (0);

    // normal positions are at location 1
    glBindBuffer (GL_ARRAY_BUFFER, vboMeshNormals);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    glEnableVertexAttribArray (1); // don't forget this!

    glBindVertexArray(0); // Disable our Vertex Buffer Object
  }

  ////////////////////////////////////////
  // associates keypoint index to Keypoint ID

  int KeypointIndex( char * KP_ID ) {
    // find current k eypoint index from keypoint ID
    // and stores it into CurrentActiveKeyPoint
    for( int ind = 0 ; ind < NbKPs ; ind++ ) {
      if( strcmp( KP_ID , TabKPs[ind].id ) == 0 ) {
	return ind;
      }
    }

    // not found
    printf("unknown KeyPoint (%s)\n" , KP_ID );
    return -1;
  }

  ////////////////////////////////////////
  // associates Bone index to Bone ID

  int BoneIndex( char * Bone_ID ) {
    for( int ind = 0 ; ind < NbBones ; ind++ ) {
      if( strcmp( Bone_ID , TabBones[ind].id ) == 0 ) {
	return ind;
      }
    }

    // not found
    printf("unknown Bone (%s)\n" , Bone_ID );
    return -1;
  }

  ////////////////////////////////////////
  // Bone OBJ file parsing
  // (inhouse format inspired from the Alias Wavefront ASCII format)

  void parse_one_Bone_obj( FILE *file , int level , char *nextTag ) {
    char    tag[512];
    char    id[512];
    char    line[512];

    strcpy( tag , nextTag );
    while( strcmp( tag , "transl" ) == 0 ) {
      float w, x, y, z;

      // Scan for Bones in this mesh
      if( NbBones >= MAX_BONES ) {
	printf( "Error: Excessive number of Bones\n" );
	throw 0;
      }

      // has read the transl/ID line

      // stores the translation values
      fgets  ( line, 512, file );
      sscanf ( line, "%f %f %f", &x , &y , &z );
      TabBones[NbBones].boneInitialTranslationMatrix
	= glm::translate( glm::mat4(1.0f),
			  glm::vec3( x , y , z ) );

      // initialRotation tag
      fgets  ( line, 512, file );
      // stores the initialRotation values
      fgets  ( line, 512, file );

      sscanf ( line, "%f %f %f %f",
	       &w , &x , &y , &z );
      glm::quat initialRotation
	= glm::quat( w , x , y , z );
      TabBones[NbBones].boneInitialRotationMatrix
	= glm::mat4_cast( initialRotation );

      // bone
      fgets  ( line, 512, file );
      sscanf ( line, "%s %s",
	       tag, TabBones[NbBones].id );
      // length
      fgets  ( line, 512, file );
      sscanf ( line, "%f",
	       &(TabBones[NbBones].length) );
      TabBones[NbBones].points[2 * 3 + 1] = TabBones[NbBones].length;
      TabBones[NbBones].points[5 * 3 + 1] = TabBones[NbBones].length;

      // parent
      fgets  ( line, 512, file );
      sscanf ( line, "%s %s",
	       tag, id );
      printf( "Bone %s (parent: %s) (prof: %d)\n",
	      TabBones[NbBones].id , id , level );

      // empty line
      fgets  ( line, 512, file );

      // associates the parent bone with the current bone
      if( strcmp( id , "NULL" ) != 0 ) {
	bool parentfound = false;
	for( int ind = 0 ; ind < NbBones ; ind++ ) {
	  if( strcmp( TabBones[ind].id , id ) == 0 ) {
	    TabBones[NbBones].parentBone = TabBones + ind;
	    if( !TabBones[ind].daughterBone ) {
	      TabBones[ind].daughterBone = TabBones + NbBones;
	    }
	    else {
	      Bone *currentBone = TabBones[ind].daughterBone;
	      while( currentBone->sisterBone ) {
		currentBone = currentBone->sisterBone;
	      }
	      currentBone->sisterBone = TabBones + NbBones;
	    }
	    parentfound = true;
	    break;
	  }
	}
	if( !parentfound ) {
	  printf( "Parent of bone %s (%s) not found!\n",
		  TabBones[NbBones].id , id  );
	}
      }
      // no parent chains with the root node
      else {
	// it is not the root node
	if( NbBones > 0 ) {
	  Bone *currentBone = TabBones;
	  while( currentBone->sisterBone ) {
	    currentBone = currentBone->sisterBone;
	  }
	  currentBone->sisterBone = TabBones + NbBones;
	}
      }

      // next tag
      fgets  ( line, 512, file );
      sscanf ( line, "%s %s", tag, id );

      NbBones++;

      // daughter bone
      if( strcmp( tag , "transl" ) == 0 ) {
	parse_one_Bone_obj( file , level + 1 , tag );
	if( strcmp( tag , "end" ) == 0 ) { strcpy( nextTag , "end" ); return; }

	// empty line: end of file
	fgets  ( line, 512, file );

	// if empty line: end of file
	if( !fgets  ( line, 512, file ) ) { strcpy( nextTag , "end" ); return; }
	// non empty line: reads further (possible sister node)
	sscanf ( line, "%s %s", tag , id );
      }

      // end_bone tag
      else if( strcmp( tag , "bone_end" ) == 0 ) {
	// empty line
	fgets  ( line, 512, file );

	// if empty line: end of file
	if( !fgets  ( line, 512, file ) ) { strcpy( nextTag , "end" ); return; }
	// non empty line: reads further (possible sister node)
	sscanf ( line, "%s %s", tag , id );
      }
    }
  }

  void parse_Bone_obj( FILE *file ) {
    char    tag[512];
    char    line[512];
    char    id[512];

    // One comment line
    // # Blender3D Bones File: Anime_Girl_Bones.blend
    fgets  ( line, 512, file );

    NbBones = 0;

    // next tag
    fgets  ( line, 512, file );
    sscanf ( line, "%s %s", tag , id );

    while( strcmp( tag , "transl" ) == 0 ) {
      parse_one_Bone_obj( file , 1 , tag );
    }
  }

};

//////////////////////////////////////////////////////////////////
// SHADERS
//////////////////////////////////////////////////////////////////
class Shader {
public:
  ////////////////////////////////////////
  // shader variable pointers
  GLint uniform_object_model;
  GLint uniform_object_inverseModel;
  GLint uniform_object_view;
  GLint uniform_object_proj;
  GLint uniform_object_light;
  GLint uniform_object_eye;
  GLint uniform_object_objectColor;
  unsigned int shader_programme;

  void init_shader( char *vertexMeshShaderName , char *fragmentMeshShaderName ) {
    ////////////////////////////////////////
    // loading and compiling shaders
    unsigned int vs = glCreateShader (GL_VERTEX_SHADER);
    loadshader( vertexMeshShaderName , vs);
    glCompileShader (vs);
    printLog(vs);

    unsigned int fs = glCreateShader (GL_FRAGMENT_SHADER);
    loadshader( fragmentMeshShaderName , fs);
    glCompileShader (fs);
    printLog(fs);

    shader_programme = glCreateProgram ();
    glAttachShader (shader_programme, fs);
    glAttachShader (shader_programme, vs);
    glLinkProgram (shader_programme);

    ////////////////////////////////////////
    // shader parameeter bindings
    uniform_object_model
      = glGetUniformLocation(shader_programme, "modelMatrix");
    uniform_object_inverseModel
      = glGetUniformLocation(shader_programme, "inverseModelMatrix");
    uniform_object_view
      = glGetUniformLocation(shader_programme, "viewMatrix");
    uniform_object_proj
      = glGetUniformLocation(shader_programme, "projectionMatrix");
    uniform_object_light
      = glGetUniformLocation(shader_programme, "light");
    uniform_object_eye
      = glGetUniformLocation(shader_programme, "eye");
    uniform_object_objectColor
      = glGetUniformLocation(shader_programme, "objectColor");
    if ( (uniform_object_proj == -1)
	 || (uniform_object_view == -1)
	 || (uniform_object_inverseModel == -1)
	 || (uniform_object_model == -1)
	 || (uniform_object_light == -1)
	 || (uniform_object_eye == -1)
	 || (uniform_object_objectColor == -1)
	 ) {
      fprintf(stderr, "Could not bind mesh uniforms %d %d %d %d %d %d %d\n" ,
	      (uniform_object_proj == -1)
	      , (uniform_object_view == -1)
	      , (uniform_object_model == -1)
	      , (uniform_object_inverseModel == -1)
	      , (uniform_object_light == -1)
	      , (uniform_object_eye == -1)
	      , (uniform_object_objectColor == -1)
	      );
    }
  }
};



























