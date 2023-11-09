#include "App.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Gui.h"
#include "SceneManager.h"

#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"


Renderer3D::Renderer3D(App* app) : Module(app)
{
}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    app->engine->Awake();

    return true;
}

bool Renderer3D::Start()
{
    app->engine->Start();

    // Creating Editor Camera GO (Outside hierarchy)
    cameraGO = std::make_shared<GameObject>("EDITOR CAMERA");
    cameraGO.get()->AddComponent(ComponentType::Transform);
    cameraGO.get()->AddComponent(ComponentType::Camera);

    // hekbas test adding same component
    cameraGO.get()->AddComponent(ComponentType::Camera);
    cameraGO.get()->GetComponent<Transform>().get()->setPosition(vec3f(0,2,-10));

    return true;
}

bool Renderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    return true;
}

bool Renderer3D::Update(double dt)
{
    CameraInput(dt);
    app->engine->Update(dt);

    return true;
}

bool Renderer3D::PostUpdate()
{ 
    std::shared_ptr<Camera> camera = cameraGO.get()->GetComponent<Camera>();
    app->engine->Render(EngineCore::RenderModes::DEBUG, camera);

    // hekbas testing Mesh load/draw
    /*static auto mesh_ptrs = MeshLoader::loadFromFile("Assets/mf.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();*/

    app->gui->Draw();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

bool Renderer3D::CleanUp()
{

    return true;
}

void Renderer3D::CameraInput(double dt)
{
    std::shared_ptr<Camera> camera = cameraGO.get()->GetComponent<Camera>();
    std::shared_ptr<Transform> transform = cameraGO.get()->GetComponent<Transform>();

    float speed = 10 * dt;
    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        speed = 20 * dt;

    float mouseSensitivity = 10.0f * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        /* MOUSE CAMERA MOVEMENT */
        float dx = -app->input->GetMouseXMotion() * mouseSensitivity;
        float dy = app->input->GetMouseYMotion() * mouseSensitivity;

        camera.get()->rotate(vec3f(0.0f, dx, 0.0f), false);
        camera.get()->rotate(vec3f(dy, 0.0f, 0.0f), true);

        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
        {
            camera.get()->translate(transform.get()->getForward() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
        {
            camera.get()->translate(-transform.get()->getForward() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
        {
            camera.get()->translate(transform.get()->getRight() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
        {
            camera.get()->translate(-transform.get()->getRight() * speed);
        }
    }
    else
    {
        // Zooming Camera Input
        camera.get()->translate(transform.get()->getForward() * (float)app->input->GetMouseZ());
    }

    // Orbit Object with Alt + LMB
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        float dx = -app->input->GetMouseXMotion() * mouseSensitivity;
        float dy = app->input->GetMouseYMotion() * mouseSensitivity;

        camera.get()->setPosition(camera.get()->center);
       
        camera.get()->rotate(vec3f(0.0f, 1.0f, 0.0f), dx, false);
        camera.get()->rotate(vec3f(1.0f, 0.0f, 0.0f), dy, true);

        vec3f finalPos;
        finalPos = transform.get()->getPosition() - transform.get()->getForward();
        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            finalPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>().get()->getPosition() - (transform.get()->getForward() * 100.0f);
        }

        camera.get()->setPosition(finalPos);
    }

    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && app->sceneManager->GetSelectedGO() != nullptr)
    {
        vec3f targetPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>().get()->getPosition() - transform.get()->getForward();

        camera.get()->setPosition(targetPos * 100.0f);
    }

    camera.get()->updateCameraVectors();
}