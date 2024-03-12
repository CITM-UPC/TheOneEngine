using System;

public class CameraMovement : MonoBehaviour
{
	IGameObject playerGO;
	Vector3 offset = new Vector3(0.0f, 32.0f, -40.0f);

	public override void Start()
	{
		playerGO = IGameObject.Find("Player");
	}

	public override void Update()
	{
		attachedGameObject.transform.position = playerGO.transform.position + offset;
	}
}
