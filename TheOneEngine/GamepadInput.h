#ifndef GAMEPAD_INPUT_H
#define GAMEPAD_INPUT_H

#include <array>
#include <unordered_map>
#include <memory>

struct SDL_ControllerDeviceEvent;
typedef struct _SDL_GameController SDL_GameController;

class GamepadInput
{
public: 
	enum class Axis
	{
        LEFTX,
        LEFTY,
        RIGHTX,
        RIGHTY,
        TRIGGERLEFT,
        TRIGGERRIGHT,
        MAX
	};

    enum class Button
    {
        A,
        B,
        X,
        Y,
        BACK,
        GUIDE,
        START,
        LEFTSTICK,
        RIGHTSTICK,
        LEFTSHOULDER,
        RIGHTSHOULDER,
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT,
        MAX
    };

public:
    static void GamepadConnected(SDL_ControllerDeviceEvent& event);
    static void GamepadDisconnected(SDL_ControllerDeviceEvent& event);
    static void Shutdown();
    static void Update();

    static bool GetButton(int ID, Button button);
    static bool GetButtonDown(int ID, Button button);
    static bool GetButtonUp(int ID, Button button);
    static float GetAxis(int ID, Axis axis);

private:
    static int GetNextFreeIndex();

private:
    struct Controller
    {
        SDL_GameController* gameController = nullptr;

        std::array<bool, (int)Button::MAX> bottons;
        std::array<bool, (int)Button::MAX> lastButtons;
        std::array<float, (int)Axis::MAX> axes; // -1 to 1
        std::array<float, (int)Axis::MAX> lastAxes;
    };

    static std::unordered_map<int, std::unique_ptr<GamepadInput::Controller>> availableGamepads;
};

#endif //GAMEPAD_INPUT_H

// SDL_GameControllerAxis
// SDL_GameControllerButton
