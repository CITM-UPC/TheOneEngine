using System;
using System.Runtime.CompilerServices;

public class MonoBehaviour
{
    public MonoBehaviour()
    {
        IntPtr gameObjectPtr = InternalCalls.GetGameObjectPtr(out gameObjectPtr);
        attachedGameObject = new IGameObject(gameObjectPtr);

        InternalCalls.PrintCSharpPtrInfo(gameObjectPtr);
    }

    public IGameObject attachedGameObject;

    public virtual void Start() { }
    public virtual void Update() { }
}
