//#include <GL/glut.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//#ifndef CALLBACK
//#define CALLBACK
//#endif
//
//GLuint startList;
//
//
//
//void CALLBACK errorCallback(GLenum errorCode) // error handler
//
//{
//
//  const GLubyte *estring;
//
//
//
//  estring = gluErrorString(errorCode);
//
//  fprintf(stderr, "Quadric Error: %b\n", estring);   // write error message to console
//
//  exit(0);
//
//}
//
//
//
//
//
//
//void createQuad(void)
//
//{
//
//  GLUquadricObj *qobj = gluNewQuadric();                  // create a new Quadric object
//
//  //gluQuadricCallback(qobj, GLU_ERROR, errorCallback);   // set callback
//  gluQuadricDrawStyle(qobj, GLU_FILL);                    // set the drawing style
//  gluQuadricNormals(qobj, GLU_SMOOTH);                    // create normals for shading    
//  gluCylinder(qobj, 0.5, 0.3, 1.0, 15, 5);                // draw a cylinder ( obj, baseRad, topRad, height, slices, stacks)
//                                                          // aligned with Z axis, base on the Z = 0 plane, baseRad/topRad are radius   
//
//
//
//}