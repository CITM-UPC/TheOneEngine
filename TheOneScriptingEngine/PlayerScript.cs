using System;

public class PlayerScript : MonoBehaviour
{
	float speed = 7.0f;

	public override void Update()
	{
		bool toMove = false;
		Vector3 movement = Vector3.zero;

		//Keyboard

		if (Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE))
		{
			InternalCalls.ExitApplication();
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.W))
		{
			movement = movement + Vector3.forward;
			toMove = true;
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.D))
		{
			movement = movement - Vector3.right;
			toMove = true;
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.S))
		{
			movement = movement - Vector3.forward;
			toMove = true;
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.A))
		{
			movement = movement + Vector3.right;
			toMove = true;
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
		{
			attachedGameObject.transform.rotation = Vector3.zero;
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
		{
			attachedGameObject.transform.rotation = new Vector3(0, 1.57f, 0);
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
		{
			attachedGameObject.transform.rotation = new Vector3(0, 4.71f, 0);
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
		{
			attachedGameObject.transform.rotation = new Vector3(0, 3.14f, 0);
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR))
		{
			InternalCalls.InstantiateBullet(attachedGameObject.transform.position, attachedGameObject.transform.rotation);
		}

		if (toMove)
		{
			attachedGameObject.transform.Translate(movement.Normalize() * speed * Time.deltaTime);
		}

		//Controller
		Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

		if (movementVector.x != 0.0f || movementVector.y != 0.0f)
		{
			movement = new Vector3(-movementVector.x, 0.0f, -movementVector.y);

			attachedGameObject.transform.Translate(movement * speed * Time.deltaTime);
		}

		Vector2 lookVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

		if (lookVector.x != 0.0f || lookVector.y != 0.0f)
		{
			float characterRotation = (float)Math.Atan2(-lookVector.x, -lookVector.y);
			attachedGameObject.transform.rotation = new Vector3(0.0f, characterRotation, 0.0f);
		}

		if (Input.GetControllerButton(Input.ControllerButtonCode.R1))
        {
			InternalCalls.InstantiateBullet(attachedGameObject.transform.position, attachedGameObject.transform.rotation);
		}
	}
}
