#include "App.h"
#include "Renderer3D.h"


Renderer3D::Renderer3D(App* app) : Module(app) {}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    app->engine->Awake();

    return true;
}

bool Renderer3D::Start()
{
    app->engine->Start();

    app->engine->camera.fov = 65;
    app->engine->camera.aspect = static_cast<double>(WINDOW_WIDTH) / WINDOW_HEIGHT;
    app->engine->camera.zNear = 0.1;
    app->engine->camera.zFar = 1500;
    app->engine->camera.eye = vec3(0, 0, 250);
    app->engine->camera.center = vec3(0, 0, -100);
    app->engine->camera.up = vec3(0, 0, 1);

    return true;
}

bool Renderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    return true;
}

bool Renderer3D::Update(double dt)
{
    app->engine->Update(dt);

    // hekbas texting camera movement
    double radius = 950;
    
    dtSum += dt;
    double speed = 0.6 * dtSum;

    app->engine->camera.eye.x = sin(speed) * radius;
    app->engine->camera.eye.y = cos(speed) * radius;

    return true;
}

bool Renderer3D::PostUpdate()
{ 
    app->engine->Render(EngineCore::RenderModes::DEBUG);

    // hekbas testing Mesh load/draw
    static auto mesh_ptrs = AssetMesh::loadFromFile("Assets/mf.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();

    app->gui->RenderGui();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

void Renderer3D::CameraMovement()
{
	//// Normalized vector of the substraction of the Camera Focus Vec and the Camera Position Vec, then multiplied by the speed we want
	//vec3 normalizedVec = (app->engine->camera.cameraSpeed * (glm::normalize(app->engine->camera.focusPosVec - app->engine->camera.worldPosVec)));


	//// Rotation matrix definition
	//mat3 rotationMat = mat3(cos(glm::radians(90.0f)), 0, sin(glm::radians(90.0f)),
	//	0, 1, 0,
	//	-sin(glm::radians(90.0f)), 0, cos(glm::radians(90.0f)));

	//// Normal vector between the rotation matrix and the normalized vector
	//vec3 normalVec = rotationMat * normalizedVec;


	///* KEYBOARD CAMERA MOVEMENT */
	//// Compute camera movement if 'MOUSE RIGHT CLICK' is pressed
	//if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	//{
	//	// Check if 'LEFT SHIFT' is pressed, then camera speed is higher (x3) times
	//	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	//	{
	//		normalizedVec *= app->engine->camera.cameraSpeedMultiplier;
	//		normalVec *= app->engine->camera.cameraSpeedMultiplier;
	//	}

	//	// Check if 'W' key is pressed
	//	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	//	{
	//		// Compute FORWARD camera movement
	//		app->engine->camera.worldPosVec += normalizedVec;
	//		app->engine->camera.focusPosVec += normalizedVec;
	//	}
	//	// Check if 'S' key is pressed
	//	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	//	{
	//		// Compute BACKWARDS camera movement
	//		app->engine->camera.worldPosVec -= normalizedVec;
	//		app->engine->camera.focusPosVec -= normalizedVec;
	//	}

	//	// Check if 'A' key is pressed
	//	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	//	{
	//		// Compute LEFT camera movement
	//		normalVec.y = 0; // Set to zero the Y component, otherwise it gets f*ucked idk why tbh
	//		app->engine->camera.worldPosVec -= normalVec;
	//		app->engine->camera.focusPosVec -= normalVec;
	//	}

	//	// Check if 'D' key is pressed
	//	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	//	{
	//		// Compute RIGHT camera movement
	//		normalVec.y = 0;
	//		app->engine->camera.worldPosVec += normalVec;
	//		app->engine->camera.focusPosVec += normalVec;
	//	}


	//	/* MOUSE CAMERA MOVEMENT */
	//	// Compute mouse input displacement
	//	int deltaX = app->input->GetMouseXMotion();
	//	int deltaY = -app->input->GetMouseYMotion();

	//	app->engine->camera.cameraYaw += deltaX * app->engine->camera.mouseSensitivity;
	//	app->engine->camera.cameraPitch += deltaY * app->engine->camera.mouseSensitivity;

	//	// Limiting Camera Piitch to prevent flipping
	//	if (app->engine->camera.cameraPitch > 89.0f)
	//		app->engine->camera.cameraPitch = 89.0f;
	//	if (app->engine->camera.cameraPitch < -89.0f)
	//		app->engine->camera.cameraPitch = -89.0f;

	//	glm::dvec3 directionVec;
	//	directionVec.x = cos(glm::radians(app->engine->camera.cameraYaw)) * cos(glm::radians(app->engine->camera.cameraPitch));
	//	directionVec.y = sin(glm::radians(app->engine->camera.cameraPitch));
	//	directionVec.z = sin(glm::radians(app->engine->camera.cameraYaw)) * cos(glm::radians(app->engine->camera.cameraPitch));

	//	// Update Camera's Focus view point vector to be recomputed in the renderer with gluLookAt()
	//	app->engine->camera.focusPosVec = app->engine->camera.worldPosVec + directionVec;
	//}
}

bool Renderer3D::CleanUp()
{

    return true;
}