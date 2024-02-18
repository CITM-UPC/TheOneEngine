#include "EngineCore.h"
#include "..\TheOneEditor\Log.h"
#include <GL\glew.h>
#include <glm\ext\matrix_transform.hpp>
#include <IL\il.h>

#include <memory>

EngineCore::EngineCore()
{
    audio = new AudioCore();
}

void EngineCore::Awake()
{
    LOG(LogType::LOG_OK, "Initializing DevIL");
    ilInit();
    audio->Awake();
}

void EngineCore::Start()
{
    
}

void EngineCore::Update(double dt)
{
    audio->Update(dt);
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

void EngineCore::CleanUp()
{
    audio->CleanUp();
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
    glColor3f(1, 1, 1);
    glLineWidth(0.5);

    // Draw near plane
    glBegin(GL_LINE_LOOP);
    glVertex3f(frustum.vertices[0].x, frustum.vertices[0].y, frustum.vertices[0].z);
    glVertex3f(frustum.vertices[1].x, frustum.vertices[1].y, frustum.vertices[1].z);
    glVertex3f(frustum.vertices[2].x, frustum.vertices[2].y, frustum.vertices[2].z);
    glVertex3f(frustum.vertices[3].x, frustum.vertices[3].y, frustum.vertices[3].z);
    glEnd();

    // Draw far plane
    glBegin(GL_LINE_LOOP);
    glVertex3f(frustum.vertices[4].x, frustum.vertices[4].y, frustum.vertices[4].z);
    glVertex3f(frustum.vertices[5].x, frustum.vertices[5].y, frustum.vertices[5].z);
    glVertex3f(frustum.vertices[6].x, frustum.vertices[6].y, frustum.vertices[6].z);
    glVertex3f(frustum.vertices[7].x, frustum.vertices[7].y, frustum.vertices[7].z);
    glEnd();

    // Draw lines connecting near and far planes
    glBegin(GL_LINES);
    glVertex3f(frustum.vertices[0].x, frustum.vertices[0].y, frustum.vertices[0].z);
    glVertex3f(frustum.vertices[4].x, frustum.vertices[4].y, frustum.vertices[4].z);

    glVertex3f(frustum.vertices[1].x, frustum.vertices[1].y, frustum.vertices[1].z);
    glVertex3f(frustum.vertices[5].x, frustum.vertices[5].y, frustum.vertices[5].z);

    glVertex3f(frustum.vertices[2].x, frustum.vertices[2].y, frustum.vertices[2].z);
    glVertex3f(frustum.vertices[6].x, frustum.vertices[6].y, frustum.vertices[6].z);

    glVertex3f(frustum.vertices[3].x, frustum.vertices[3].y, frustum.vertices[3].z);
    glVertex3f(frustum.vertices[7].x, frustum.vertices[7].y, frustum.vertices[7].z);
    glEnd();
}

void EngineCore::DrawRay(const Ray& ray)
{
    uint magnitude = 1000;
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_LINES);
    glVertex3f(ray.Origin.x, ray.Origin.y, ray.Origin.z);
    glVertex3f(ray.Direction.x * magnitude, ray.Direction.y * magnitude, ray.Direction.z * magnitude);
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

//heakbs - parameter not used?
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