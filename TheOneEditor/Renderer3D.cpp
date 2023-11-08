#include "App.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Gui.h"

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
    cameraGO = std::make_shared<GameObject>("[EDITOR CAMERA]");
    cameraGO.get()->AddComponent(ComponentType::Transform);
    cameraGO.get()->AddComponent(ComponentType::Camera);

    app->engine->Start();
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
    static auto mesh_ptrs = AssetMesh::loadFromFile("Assets/mf.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();

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

    // Implement a debug camera with keys and mouse
    // Now we can make this movememnt frame rate independant!
    float speed = 10 * dt;
    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        speed = 20 * dt;

    float mouseSensitivity = 10.0f * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        /* MOUSE CAMERA MOVEMENT */
        // Compute mouse input displacement

        camera.get()->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera.get()->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

        camera.get()->rotate(vec3f(camera.get()->pitch, camera.get()->yaw, 0.0f), false);

        //LOG("Yaw: %f, Pitch: %f", app->engine->camera.yaw, app->engine->camera.pitch);
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
        //LOG("MouseZ: %f", (float)GetMouseZ());
    }

    // Orbit Object with Alt + LMB
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        //Get selected GameObject         
        float radius = 500.0f;

        camera.get()->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera.get()->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

        vec3f finalPos = transform.get()->getPosition() - transform.get()->getForward();

        /* app->engine->camera.transform.setPosition(vec3f(glm::cos(glm::radians(app->engine->camera.yaw)) * glm::cos(glm::radians(app->engine->camera.pitch)) * radius,
             glm::sin(glm::radians(app->engine->camera.pitch)) * radius,
             glm::sin(glm::radians(app->engine->camera.pitch)) * glm::cos(glm::radians(app->engine->camera.pitch)) * radius));*/

             //app->engine->camera.transform.setPosition(vec3f(radius * glm::cos(glm::radians(angle))));
        camera.get()->rotate(vec3f(0.0f, 1.0f, 0.0f), camera.get()->yaw, true);
        camera.get()->rotate(vec3f(1.0f, 0.0f, 0.0f), camera.get()->pitch, true);
        //glm::lerp(app->engine->camera.transform.getRotation(), )
    }

    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
    {

    }

    camera.get()->updateCameraVectors();
}