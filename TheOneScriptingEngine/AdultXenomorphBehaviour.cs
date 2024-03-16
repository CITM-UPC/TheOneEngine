using System;

public class AdultXenomorphBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Death
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    float movementSpeed = 25.0f;

    States currentState = States.Idle;
    bool detected = false;
    
    float enemyDetectedRange = 20.0f;
    float maxAttackRange = 30.0f;
    float maxChasingRange = 40.0f;

    public override void Start()
	{
		playerGO = IGameObject.Find("Player");
	}

	public override void Update()
	{
        //Draw debug ranges
        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, enemyDetectedRange,    new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange,       new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxAttackRange,        new Vector3(0.0f, 0.8f, 1.0f)); //Blue

        //Set the director vector and distance to the player
        directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

        //Change the states
        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        {
            if (playerDistance < maxAttackRange)
            {
                currentState = States.Attack;
            }
            else if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
            {
                currentState = States.Chase;
            }
            else if (playerDistance > maxChasingRange)
            {
                detected = false;
                currentState = States.Idle;
            }
        }

        switch (currentState)
        {
            case States.Idle:
                return;
            case States.Attack:
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                break;
            case States.Chase:
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                break;
            case States.Patrol:
                break;
            case States.Death:
                break;
            default:
                break;
        }
    }
}