#include "App.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Gui.h"
#include "PanelScene.h"
#include "SceneManager.h"

#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\EngineCore.h"



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
    sceneCamera = std::make_shared<GameObject>("EDITOR CAMERA");
    Transform* camera_transform = (Transform*)sceneCamera.get()->AddComponent<Transform>();
    sceneCamera.get()->AddComponent<Camera>();
    camera_transform->setPosition(vec3f(0, 15, -70));
    camera_transform->updateMatrix();

    CreateCheckersTexture();

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
    app->gui->panelScene->isHovered = false;

    app->engine->Update(dt);

    return true;
}

bool Renderer3D::PostUpdate()
{
    // Scene camera
    /*Camera* sceneCam = sceneCamera.get()->GetComponent<Camera>();
    app->engine->Render(sceneCam);*/

    // hekbas testing Mesh load/draw
    /*static auto mesh_ptrs = MeshLoader::LoadMesh("Assets/mf.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();*/

    app->gui->Draw();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

bool Renderer3D::CleanUp()
{

    return true;
}

void Renderer3D::CreateRay()
{
    //if (app->sceneManager->GetGameObjects().empty())
    //    return;

    ////App->renderer3D->SetCameraToDraw(fake_camera);
    //float2 origin = float2((App->input->GetMousePosition().x - App->ui->panel_scene->posX) / App->ui->panel_scene->width, (App->input->GetMousePosition().y - App->ui->panel_scene->posY) / App->ui->panel_scene->height);

    //origin.x = (origin.x - 0.5F) * 2;
    //origin.y = -(origin.y - 0.5F) * 2;

    //if (origin.x > 1 || origin.x < -1 || origin.y > 1 || origin.y < -1)
    //    return;

    //ray = fake_camera->frustum.UnProjectLineSegment(origin.x, origin.y);

    //std::vector<std::pair<float, GameObject*>> hits;

    //// with octree to static objects
    //CreateObjectsHitMap(&hits, App->objects->octree.root, ray);

    //// without octree for the dynamics
    //std::vector<GameObject*>::iterator item = App->objects->GetRoot(true)->children.begin();

    //for (; item != App->objects->GetRoot(true)->children.end(); ++item)
    //{
    //    if (*item != nullptr && (*item)->IsEnabled()) {
    //        CreateObjectsHitMap(&hits, (*item), ray);
    //    }
    //}
    //// sort by pos
    //std::sort(hits.begin(), hits.end(), ModuleCamera3D::SortByDistance);
    //static bool hit = false;
    //std::vector<std::pair<float, GameObject*>>::iterator it = hits.begin();

    //for (; it != hits.end(); ++it)
    //{
    //    if ((*it).second != nullptr) {
    //        if (TestTrianglesIntersections((*it).second, ray)) {
    //            hit = true;
    //            break;
    //        }
    //    }
    //}

    //if (!hit)
    //    App->objects->DeselectObjects();

    //hit = false;

}

void Renderer3D::CreateCheckersTexture()
{
    // Create the checkers data
    const unsigned int k_tex_width = 32;
    const unsigned int k_tex_height = 32;
    GLubyte checkers_texture[k_tex_height][k_tex_width][4];

	for (int i = 0; i < k_tex_height; i++) {
		for (int j = 0; j < k_tex_width; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkers_texture[i][j][0] = (GLubyte)c;
			checkers_texture[i][j][1] = (GLubyte)c;
			checkers_texture[i][j][2] = (GLubyte)c;
			checkers_texture[i][j][3] = (GLubyte)255;
		}
	}

    // OpenGL Stuff
    glGenTextures(1, &checkers_tex_);
    glBindTexture(GL_TEXTURE_2D, checkers_tex_);

    // Set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Send pixels to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, k_tex_width, k_tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkers_texture);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer3D::CameraInput(double dt)
{
    if (!app->gui->panelScene->isHovered || app->IsPlaying())
        return;

    Camera* camera = sceneCamera.get()->GetComponent<Camera>();
    Transform* transform = sceneCamera.get()->GetComponent<Transform>();

    double speed = 20 * dt;
    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        speed = 35 * dt;

    double mouseSensitivity = 18.0 * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        /* MOUSE CAMERA MOVEMENT */
        camera->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

        camera->setRotation(vec3f(camera->pitch, camera->yaw, 0.0f));

        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
        {
            camera->translate(transform->getForward() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
        {
            camera->translate(-transform->getForward() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
        {
            camera->translate(transform->getRight() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
        {
            camera->translate(-transform->getRight() * speed);
        }
    }
    else
    {
        // Zooming Camera Input
        camera->translate(transform->getForward() * (double)app->input->GetMouseZ());
    }

    // Orbit Object with Alt + LMB
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        camera->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

        camera->setPosition(camera->center);
       
        camera->rotate(vec3f(0.0f, 1.0f, 0.0f), camera->yaw, false);
        camera->rotate(vec3f(1.0f, 0.0f, 0.0f), camera->pitch, true);

        vec3f finalPos;
        finalPos = transform->getPosition() - transform->getForward();
        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            finalPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->getPosition() - (transform->getForward() * 100.0);
        }

        camera->setPosition(finalPos);
    }

    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && app->sceneManager->GetSelectedGO() != nullptr)
    {
        vec3f targetPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->getPosition() - transform->getForward();

        camera->setPosition(targetPos * 100.0f);
    }

    camera->updateCameraVectors();
}