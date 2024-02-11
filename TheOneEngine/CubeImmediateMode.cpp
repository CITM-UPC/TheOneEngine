#include "CubeImmediateMode.h"
#include <GL/glew.h>

static void drawQuadFaceTriangles(vec3 a, vec3 b, vec3 c, vec3 d) {
    glTexCoord2d( 0, 1);
    glVertex3dv(&a.x);
    glTexCoord2d(1, 1);
    glVertex3dv(&b.x);
    glTexCoord2d(1, 0);
    glVertex3dv(&c.x);

    glTexCoord2d(1, 0);
    glVertex3dv(&c.x);
    glTexCoord2d(0, 0);
    glVertex3dv(&d.x);
    glTexCoord2d(0, 1);
    glVertex3dv(&a.x);
}

CubeImmediateMode::CubeImmediateMode() : texture("checkers1.png") {}

void CubeImmediateMode::draw() {

    glEnable(GL_TEXTURE_2D);
    texture.bind();

    glColor4ub(255, 255, 255, 255);

    glBegin(GL_TRIANGLES);
    //front
    glNormal3d(0, 0, 1);
    glColor3dv(&red.x);
    drawQuadFaceTriangles(a, b, c, d);
    //back
    glNormal3d(0, 0, -1);
    glColor3dv(&green.x);
    drawQuadFaceTriangles(h, g, f, e);
    //left
    glNormal3d(-1, 0, 0);
    glColor3dv(&blue.x);
    drawQuadFaceTriangles(e, a, d, h);
    //right
    glNormal3d(1, 0, 0);
    glColor3dv(&yellow.x);
    drawQuadFaceTriangles(b, f, g, c);
    //top
    glNormal3d(0, 1, 0);
    glColor3dv(&white.x);
    drawQuadFaceTriangles(d, c, g, h);
    //bottom
    glNormal3d(0, -1, 0);
    glColor3dv(&black.x);
    drawQuadFaceTriangles(b, a, e, f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
