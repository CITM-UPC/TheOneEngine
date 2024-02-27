using System;
using System.Runtime.CompilerServices;

public class MonoBehaviour
{
    public MonoBehaviour()
    {
        ulong uid;
        InternalCalls.GetUID(out uid);
        Console.WriteLine("Received UUID is: " + uid);
        attachedGameObject = new IGameObject((ulong)uid);
    }

    public IGameObject attachedGameObject;

    public virtual void Start() { }
    public virtual void Update() { }
}
