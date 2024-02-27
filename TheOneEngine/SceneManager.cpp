#include "SceneManager.h"
#include "GameObject.h"

SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0)
{

}

SceneManager::~SceneManager()
{
    delete meshLoader;
}

bool SceneManager::Awake()
{

    return true;
}

bool SceneManager::Start()
{

    return true;
}

bool SceneManager::PreUpdate()
{
    return true;
}

bool SceneManager::Update(double dt)
{
    

    return true;
}

bool SceneManager::PostUpdate()
{
    //RecurseDrawChildren(rootSceneGO);

    return true;
}

bool SceneManager::CleanUp()
{
    return true;
}


void SceneManager::RecurseDrawChildren(std::shared_ptr<GameObject> parentGO)
{
    for (const auto gameObject : parentGO.get()->children)
    {
        gameObject.get()->Draw();
        RecurseDrawChildren(gameObject);
    }
}
