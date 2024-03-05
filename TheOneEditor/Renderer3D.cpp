#include "App.h"

#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "Gui.h"
#include "PanelScene.h"

#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\Listener.h"
#include "..\TheOneEngine\EngineCore.h"

Renderer3D::Renderer3D(App* app) : Module(app)
{
}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    engine->Awake();

	return true;
}

bool Renderer3D::Start()
{
    engine->Start();

    // Creating Editor Camera GO (Outside hierarchy)
    sceneCamera = std::make_shared<GameObject>("EDITOR CAMERA");
    sceneCamera.get()->AddComponent<Transform>();
    sceneCamera.get()->GetComponent<Transform>()->SetPosition(vec3f(0, 3, -10));
    sceneCamera.get()->AddComponent<Camera>();
    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();

    //// JULS: Listener 
    //sceneCamera.get()->AddComponent<Listener>();
	//sceneCamera.get()->GetComponent<Listener>()->goID = audioManager->audio->RegisterGameObject(sceneCamera.get()->GetName());
	//audioManager->AddAudioObject((std::shared_ptr<AudioComponent>)sceneCamera.get()->GetComponent<Listener>());
	//audioManager->audio->SetDefaultListener(sceneCamera.get()->GetComponent<Listener>()->goID);

    // hekbas test adding same component
    LOG(LogType::LOG_INFO, "# Testing Component Duplication");
    sceneCamera.get()->AddComponent<Camera>();
	//sceneCamera.get()->AddComponent<Listener>();
	cameraParent = std::make_shared<GameObject>("CameraParent");
	cameraParent.get()->AddComponent<Transform>();
	cameraParent.get()->children.push_back(sceneCamera);
	sceneCamera.get()->parent = cameraParent;


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

    engine->Update(dt);

    // JULS: Update should be done in the Audio Manager but for now I will leave it here
    //sceneCamera.get()->GetComponent<Listener>()->SetTransform(sceneCamera);

    //app->engine->audio->SetAudioGameObjectTransform(
    //    //here must the gameobject compoment -> wwisegameobject   of the camera
    //    sceneCamera.get()->GetComponent<Transform>()->getPosition().x,
    //    sceneCamera.get()->GetComponent<Transform>()->getPosition().y,
    //    sceneCamera.get()->GetComponent<Transform>()->getPosition().z,
    //    sceneCamera.get()->GetComponent<Transform>()->getForward().x,
    //    sceneCamera.get()->GetComponent<Transform>()->getForward().y,
    //    sceneCamera.get()->GetComponent<Transform>()->getForward().z,
    //    sceneCamera.get()->GetComponent<Transform>()->getUp().x,
    //    sceneCamera.get()->GetComponent<Transform>()->getUp().y,
    //    sceneCamera.get()->GetComponent<Transform>()->getUp().z);

    //app->engine->audio->SetAudioGameObjectPosition(
    //    //here must the gameobject compoment -> wwisegameobject   of the spatial sound 1
    //    app->sceneManager->spatialObject1->GetComponent<Transform>()->getPosition().x,
    //    app->sceneManager->spatialObject1->GetComponent<Transform>()->getPosition().y,
    //    app->sceneManager->spatialObject1->GetComponent<Transform>()->getPosition().z);
    //app->engine->audio->SetAudioGameObjectPosition(
    //    //here must the gameobject compoment -> wwisegameobject   of the spatial sound 2
    //    app->sceneManager->spatialObject2->GetComponent<Transform>()->getPosition().x,
    //    app->sceneManager->spatialObject2->GetComponent<Transform>()->getPosition().y,
    //    0);

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
    engine->CleanUp();
    return true;
}

void Renderer3D::CameraInput(double dt)
{
	if (!app->gui->panelScene->isHovered)
		return;

	Camera* camera = sceneCamera.get()->GetComponent<Camera>();
	Transform* transform = sceneCamera.get()->GetComponent<Transform>();

	double speed = 20 * dt;
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 35 * dt;

    double mouseSensitivity = 36.0 * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        // Yaw and Pitch
        camera->yaw = app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch = -app->input->GetMouseYMotion() * mouseSensitivity;

        transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
        transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

        // WASD
        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			transform->Translate(transform->GetForward() * speed, HandleSpace::LOCAL);
        
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			transform->Translate(-transform->GetForward() * speed, HandleSpace::LOCAL);
        
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			transform->Translate(transform->GetRight() * speed, HandleSpace::LOCAL);
        
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			transform->Translate(-transform->GetRight() * speed, HandleSpace::LOCAL);
        
    }
    else if (app->input->GetMouseZ() != 0) 
    {
        // Zoom
		transform->Translate(transform->GetForward() * (double)app->input->GetMouseZ());
    }

    // (MMB) Panning
    if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE))
    {
        double deltaX = app->input->GetMouseXMotion();
        double deltaY = app->input->GetMouseYMotion();

        float panSpeed = 10 * dt;

        transform->Translate(vec3(deltaX * panSpeed, 0, 0), HandleSpace::GLOBAL);
        transform->Translate(vec3(0, deltaY * panSpeed, 0), HandleSpace::GLOBAL);
    }

    // (F) Focus Selection
    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && app->scenemanager->N_sceneManager->GetSelectedGO() != nullptr)
    {
        transform->SetPosition(camera->lookAt);
        vec3f finalPos;
        finalPos = transform->GetPosition() - transform->GetForward();
        finalPos = app->scenemanager->N_sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 10.0);

        transform->SetPosition(finalPos);
    }

    // ALT
    // (Alt + LMB) Orbit Selection
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        camera->yaw = app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch = -app->input->GetMouseYMotion() * mouseSensitivity;

		transform->SetPosition(camera->lookAt);
       
        transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
        transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

        vec3f finalPos;
        
        if (app->scenemanager->N_sceneManager->GetSelectedGO() != nullptr)
        {
            finalPos = app->scenemanager->N_sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 40.0);
        }
        else
        {
            finalPos = transform->GetPosition() - transform->GetForward();
        }

		transform->SetPosition(finalPos);
    }
}