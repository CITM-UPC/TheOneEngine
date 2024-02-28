using System;

public class Class1 : MonoBehaviour
{

    public override void Update()
    {
        if (attachedGameObject.transform.position.x >= 3.0f)
        {
            Console.WriteLine("Current X pos is over 3.0 and has value: " + attachedGameObject.transform.position.x);
        }
        else if (attachedGameObject.transform.position.x < 0f)
        {
            attachedGameObject.transform.Move(new Vector3(0.01f, 0, 0));
        }
    }
}
