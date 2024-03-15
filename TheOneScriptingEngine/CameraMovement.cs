﻿using System;

public class CameraMovement : MonoBehaviour
{
	IGameObject playerGO;
	Vector3 camOffset = new Vector3(0.0f, 32.0f, -40.0f);
	float camJoyDisplacement = 10.0f;

	public override void Start()
	{
		playerGO = IGameObject.Find("Player");
	}

	public override void Update()
	{
		attachedGameObject.transform.position = playerGO.transform.position + camOffset;


		Vector2 lookVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

		if (Math.Abs(lookVector.x) > 0.1f || Math.Abs(lookVector.y) > 0.1f)
		{
			Vector3 cameraDisplacement = new Vector3(-lookVector.x, 0.0f, -lookVector.y);
			attachedGameObject.transform.position += cameraDisplacement * camJoyDisplacement;
		}
	}
}
