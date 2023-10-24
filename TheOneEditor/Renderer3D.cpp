#include "Renderer3D.h"
#include "App.h"

// hekbas testing Mesh load/draw
#include "AssetMesh.h"


Renderer3D::Renderer3D(App* app) : Module(app) {}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{

    return true;
}

bool Renderer3D::Start()
{
    engine.camera.fov = 60;
    engine.camera.aspect = static_cast<double>(WINDOW_WIDTH) / WINDOW_HEIGHT;
    engine.camera.zNear = 0.1;
    engine.camera.zFar = 100;
    engine.camera.eye = vec3(5, 1.75, 5);
    engine.camera.center = vec3(0, 1, 0);
    engine.camera.up = vec3(0, 1, 0);

    return true;
}

bool Renderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    return true;
}

bool Renderer3D::Update(double dt)
{
    engine.step(dt);

    return true;
}

bool Renderer3D::PostUpdate()
{ 
    engine.render(TheOneEngine::RenderModes::DEBUG);

    // hekbas testing Mesh load/draw
    static auto mesh_ptrs = AssetMesh::loadFromFile("Assets/SM_BakerHouse.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();

    app->gui->RenderGui();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

bool Renderer3D::CleanUp()
{

    return true;
}