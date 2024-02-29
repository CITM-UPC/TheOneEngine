using System;
using System.Runtime.CompilerServices;

public class MonoBehaviour
{
    public IGameObject attachedGameObject;

    public MonoBehaviour()
    {
        attachedGameObject = new IGameObject();
    }

    public virtual void Start() { }
    public virtual void Update() { }
}
