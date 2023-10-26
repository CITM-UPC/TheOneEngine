#include "EngineCore.h"
#include <GL\glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <IL/il.h>

EngineCore::EngineCore()
{
    
}

void EngineCore::Awake()
{
    
}

void EngineCore::Start()
{
    
}

void EngineCore::Update(double dt)
{

}

static void drawAxis()
{
    glLineWidth(4.0);
    glBegin(GL_LINES);
    glColor3ub(255, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0.8, 0, 0);
    glColor3ub(0, 255, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0.8, 0);
    glColor3ub(0, 0, 255);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 0.8);
    glEnd();
}

static void drawGrid(int grid_size, int grid_step)
{
    glLineWidth(1.0);
    glColor3ub(128, 128, 128);

    glBegin(GL_LINES);
    for (int i = -grid_size; i <= grid_size; i += grid_step)
    {
        //XY plane
        /*glVertex2i(i, -grid_size);
        glVertex2i(i,  grid_size);
        glVertex2i(-grid_size, i);
        glVertex2i( grid_size, i);*/

        //XZ plane
        glVertex3i(i, 0, -grid_size);
        glVertex3i(i, 0, grid_size);
        glVertex3i(-grid_size, 0, i);
        glVertex3i( grid_size, 0, i);
    }
    glEnd();
}

void EngineCore::Render(RenderModes renderMode)
{  
    ilInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_LIGHTING);

    gluPerspective(camera.fov, camera.aspect, camera.zNear, camera.zFar);
    
    gluLookAt( camera.eye.x, camera.eye.y, camera.eye.z,
        camera.center.x, camera.center.y, camera.center.z,
        camera.up.x, camera.up.y, camera.up.z);

    drawGrid(100, 1);
    drawAxis();
    

    assert(glGetError() == GL_NONE);
}