#include "App.h"
#include "SceneManager.h"
#include "Renderer3D.h"
#include "Gui.h"
#include "PanelInspector.h"
#include "../TheOneEngine/Log.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app) {}

SceneManager::~SceneManager() {}

bool SceneManager::Awake()
{
	N_sceneManager = new N_SceneManager();

	return true;
}

bool SceneManager::Start()
{
	N_sceneManager->currentScene = new Scene(0, "NewUntitledScene");
	N_sceneManager->CreateCameraGO("mainCamera");
	//N_sceneManager->LoadScene("Scene_2");
	N_sceneManager->Start();

	N_sceneManager->CreateTeapot();
	engine->monoManager->bulletGO = N_sceneManager->currentScene->GetRootSceneGO()->children.back().get();
	engine->monoManager->bulletGO->AddScript("Bullet");
	engine->monoManager->bulletGO->Disable();

	return true;


    // JULS: Commented everything related to old SceneManager 
    /*
    // hekbas testing creation of GO
    
    //CreateEmptyGO();
    //CreateCube();
    //CreateSphere();
    
    CreateMeshGO("Assets\\Meshes\\baker_house.fbx");
    /*CreateMeshGO("Assets\\Meshes\\street_new.fbx");
    CreateMeshGO("Assets\\Meshes\\Cadillac_CT4_V_2022.fbx");

    for (auto mesh : GetGameObjects()) {
        if (mesh->GetName() == "Cadillac_CT4_V_2022_LowPoly") {
            demo = mesh;
        }
    }

    rotationAngle = 0.0f;
    rotationSpeed = 30.0f;

    std::shared_ptr<GameObject> gameCam = CreateCameraGO("Game Camera");
    gameCam.get()->GetComponent<Camera>()->setPosition({ -10, 8, 0 });

    spatialObject2 = std::shared_ptr<GameObject>(CreateTeapot("Assets\\Meshes\\teapot.fbx").get());
    spatialObject2->GetComponent<Transform>()->setPosition({ -50,15,0 });
    spatialObject2->AddComponent<Source>();

    spatialObject1 = std::shared_ptr<GameObject>(CreateTeapot("Assets\\Meshes\\teapot.fbx").get());
    spatialObject1->GetComponent<Transform>()->setPosition({ 50,15,0 });
    spatialObject1->AddComponent<Source>();

    return true;
    */
}

bool SceneManager::PreUpdate()
{
	N_sceneManager->PreUpdate();

	return true;
    
    /*
    //static const AkUniqueID MUSIC1 = 1730564819U;
    //static const AkUniqueID MUSIC2 = 1730564816U;
    //static const AkUniqueID SPATIAL1 = 2134552412U;
    //static const AkUniqueID SPATIAL2 = 2134552415U;
    //testing
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
    {
        audio->PlayEvent(2134552412U, 1);
    }
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
    {
        audio->StopEvent(2134552412U, 1);
    }
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
    {
        audio->PauseEvent(2134552412U, 1);
    }
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
    {
        audio->ResumeEvent(2134552412U, 1);
    }


    // JULS: For the UI related to Audio from teh Engine and Game
    if (app->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_REPEAT)
    {
        audio->SetGlobalVolume(audio->globalVolume - 2);
    }
    if (app->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_REPEAT)
    {
        audio->SetGlobalVolume(audio->globalVolume + 2);
    }

    return true;*/
}

bool SceneManager::Update(double dt)
{
	N_sceneManager->Update(dt, app->IsPlaying());

    /*if (app->IsPlaying()) {
        demo->GetComponent<Transform>()->rotate({ 0, 1, 0 }, rotationAngle);

        rotationAngle += rotationSpeed * dt;

        if (rotationAngle >= 360.0f)
            rotationAngle -= 360.0f;
    }

    if (app->state == GameState::NONE) {
        demo->GetComponent<Transform>()->rotate({ 1, 0, 0 }, 0.0);
        rotationAngle = 0.0;
        spatialObject2->GetComponent<Transform>()->setPosition({ -50,15,0 });
    }

    spatialObject2->GetComponent<Source>()->SetTransform(std::shared_ptr<GameObject>(spatialObject2));
    spatialObject1->GetComponent<Source>()->SetTransform(std::shared_ptr<GameObject>(spatialObject2));
*/
    return true;
}

bool SceneManager::PostUpdate()
{
	N_sceneManager->PostUpdate();

	return true;
}

bool SceneManager::CleanUp()
{
	N_sceneManager->CleanUp();
    
    return true;
}