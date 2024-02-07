#include "EngineCore.h"
#include "..\TheOneEditor\Log.h"
#include <GL\glew.h>
#include <glm\ext\matrix_transform.hpp>
#include <IL\il.h>

#include <memory>

EngineCore::EngineCore()
{
}

void EngineCore::Awake()
{
    LOG(LogType::LOG_OK, "Initializing DevIL");
    ilInit();
}

void EngineCore::Start()
{
    
}

void EngineCore::Update(double dt)
{

}

void EngineCore::Render(Camera* camera)
{
    //glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

    // Update Camera Matrix
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

    gluPerspective(camera->fov, camera->aspect, camera->zNear, camera->zFar);

    gluLookAt(camera->eye.x, camera->eye.y, camera->eye.z,
        camera->center.x, camera->center.y, camera->center.z,
        camera->up.x, camera->up.y, camera->up.z);

    DrawGrid(1000, 10);
    DrawAxis();

    //DrawFrustum(camera->viewMatrix);

    assert(glGetError() == GL_NONE);
}

void EngineCore::DrawAxis()
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

void EngineCore::DrawGrid(int grid_size, int grid_step)
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

void EngineCore::DrawFrustum(const Frustum& frustum)
{
    // Draw lines for the frustum planes
    glBegin(GL_LINES);

    // Draw lines for the near plane
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3fv(glm::value_ptr(frustum.nearPlane.normal * float(frustum.nearPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.right.normal * float(frustum.right.distance)));

    glVertex3fv(glm::value_ptr(frustum.nearPlane.normal * float(frustum.nearPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.left.normal * float(frustum.left.distance)));

    glVertex3fv(glm::value_ptr(frustum.nearPlane.normal * float(frustum.nearPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.top.normal * float(frustum.top.distance)));

    glVertex3fv(glm::value_ptr(frustum.nearPlane.normal * float(frustum.nearPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.bot.normal * float(frustum.bot.distance)));

    // Draw lines for the far plane
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.right.normal * float(frustum.right.distance)));

    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.left.normal * float(frustum.left.distance)));

    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.top.normal * float(frustum.top.distance)));

    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));
    glVertex3fv(glm::value_ptr(frustum.bot.normal * float(frustum.bot.distance)));

    // Draw lines connecting near and far planes
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3fv(glm::value_ptr(frustum.right.normal * float(frustum.right.distance)));
    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));

    glVertex3fv(glm::value_ptr(frustum.left.normal * float(frustum.left.distance)));
    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));

    glVertex3fv(glm::value_ptr(frustum.top.normal * float(frustum.top.distance)));
    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));

    glVertex3fv(glm::value_ptr(frustum.bot.normal * float(frustum.bot.distance)));
    glVertex3fv(glm::value_ptr(frustum.farPlane.normal * float(frustum.farPlane.distance)));

    glEnd();
}

void EngineCore::OnWindowResize(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool EngineCore::GetVSync()
{
    return vsync;
}

bool EngineCore::SetVSync(bool vsync)
{
    if (this->vsync)
    {
        if (SDL_GL_SetSwapInterval(1) == -1)
        {
            LOG(LogType::LOG_ERROR, "Unable to turn on V-Sync: %s", SDL_GetError());
            return false;
        }
    }
    else
    {
        if (SDL_GL_SetSwapInterval(0) == -1)
        {
            LOG(LogType::LOG_ERROR, "Unable to turn off V-Sync: %s", SDL_GetError());
            return false;
        }
    }

    return true;
}