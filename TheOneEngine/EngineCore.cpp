#include "EngineCore.h"
#include "..\TheOneEditor\Log.h"
#include <GL\glew.h>
#include <glm\ext\matrix_transform.hpp>
#include <IL\il.h>
#include <string>

EngineCore::EngineCore()
{
}

void EngineCore::Awake()
{
    LOG(LogType::LOG_OK, "Initializing DevIL");
    ilInit();
    SetScriptEngine();
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

void EngineCore::DrawFrustum(const glm::mat4& viewProjectionMatrix)
{
    // Extract frustum vertices from the view-projection matrix
    glm::vec4 frustumVertices[8] =
    {
        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
        glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
        glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
        glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    };

    // Transform frustum vertices by the view-projection matrix
    for (int i = 0; i < 8; ++i)
    {
        frustumVertices[i] = viewProjectionMatrix * frustumVertices[i];
        frustumVertices[i] /= frustumVertices[i].w; // Perspective division
    }

    // Draw frustum ---
    glBegin(GL_LINES);

    // Near plane
    for (int i = 0; i < 4; ++i) {
        glVertex3f(frustumVertices[i].x, frustumVertices[i].y, frustumVertices[i].z);
        glVertex3f(frustumVertices[(i + 1) % 4].x, frustumVertices[(i + 1) % 4].y, frustumVertices[(i + 1) % 4].z);
    }

    // Far plane
    for (int i = 4; i < 8; ++i) {
        glVertex3f(frustumVertices[i].x, frustumVertices[i].y, frustumVertices[i].z);
        glVertex3f(frustumVertices[(i + 1) % 4 + 4].x, frustumVertices[(i + 1) % 4 + 4].y, frustumVertices[(i + 1) % 4 + 4].z);
    }

    // Lines connecting near and far planes
    for (int i = 0; i < 4; ++i) {
        glVertex3f(frustumVertices[i].x, frustumVertices[i].y, frustumVertices[i].z);
        glVertex3f(frustumVertices[i + 4].x, frustumVertices[i + 4].y, frustumVertices[i + 4].z);
    }

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

void EngineCore::SetScriptEngine() {

    if (dllHandle) {
        FreeLibrary(static_cast<HMODULE>(dllHandle));
    }

    
#ifdef _DEBUG
    const std::string dllFolder("../x64/Debug/");
#else
    const std::string dllFolder("../x64/Release/");
#endif
    dllHandle = LoadLibraryA((dllFolder + "TheOneScripting" + ".dll").data());

}