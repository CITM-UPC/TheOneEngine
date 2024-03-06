using System;

public class PlayerScript : MonoBehaviour
{
	float speed = 7.0f;

	public override void Update()
	{
		bool toMove = false;
		Vector3 movement = Vector3.zero;

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

		if (Input.GetKeyboardButton(Input.KeyboardCode.E))
		{
			InternalCalls.CreateBullet(attachedGameObject.transform.position, attachedGameObject.transform.rotation);
		}

		if (toMove)
		{
			attachedGameObject.transform.Translate(movement.Normalize() * speed * Time.deltaTime);
		}
	}
}
