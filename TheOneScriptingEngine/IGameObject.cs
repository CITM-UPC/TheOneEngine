using System;
using System.Runtime.CompilerServices;

public class IGameObject
{
    private IntPtr containerGOptr;

    public string name;
    public bool isActive;
    public ITransform transform;

    public IGameObject()
    {
        containerGOptr = InternalCalls.GetGameObjectPtr();

        transform = new ITransform(containerGOptr);
    }
}