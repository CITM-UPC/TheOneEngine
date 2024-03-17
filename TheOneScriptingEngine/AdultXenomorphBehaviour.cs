using System;

public class AdultXenomorphBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Dead
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    float life = 200;

    float movementSpeed = 35.0f;

    States currentState = States.Idle;
    bool detected = false;
    
    float enemyDetectedRange = 35.0f;
    float maxAttackRange = 20.0f;
    float maxChasingRange = 40.0f;

    bool shooting = false;
    bool hasShot = false;
    float currentTimer = 0.0f;
    float attackCooldown = 2.0f;

    public override void Start()
	{
		playerGO = IGameObject.Find("Player");
	}

	public override void Update()
	{
        if (currentState == States.Dead) return;

        //Draw debug ranges
        if (!detected)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, enemyDetectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
        }

        //Set the director vector and distance to the player
        directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

        UpdateFSMStates();
        DoStateBehaviour();
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected && !shooting)
        {
            if (playerDistance < maxAttackRange)
            {
                shooting = true;
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
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                return;
            case States.Attack:
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                if (currentTimer < attackCooldown)
                {
                    currentTimer += Time.deltaTime;
                    if (!hasShot && currentTimer > attackCooldown / 2)
                    {
                        InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 4.5f, attachedGameObject.transform.rotation);
                        hasShot = true;
                    }
                    break;
                }
                currentTimer = 0.0f;
                hasShot = false;
                shooting = false;
                break;
            case States.Chase:
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                break;
            case States.Patrol:
                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f); //80 degrees??
                break;
            default:
                break;
        }
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 50.0f;
    }
}