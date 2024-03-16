using System;

public class AdultXenomorphBehaviour : MonoBehaviour
{
    IGameObject playerGO;
    Vector3 directorVector;

    bool chasing = false;
    float movementSpeed = 15.0f;
    float detectionRange = 20.0f;

    public override void Start()
	{
		playerGO = IGameObject.Find("Player");
	}

	public override void Update()
	{
        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, detectionRange, new Vector3(0.9f, 0.0f, 0.9f));

        directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();

        if (!chasing && Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position) < detectionRange)
        {
            chasing = true;
        }

        if (!chasing) return;

        attachedGameObject.transform.LookAt(playerGO.transform.position);
        attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
    }
}