using System;
using System.Runtime.CompilerServices;

public class MonoBehaviour
{
    public MonoBehaviour()
    {
        attachedGameObject = new IGameObject();
    }

    public IGameObject attachedGameObject;

    public virtual void Start() { }
    public virtual void Update() { }
}
