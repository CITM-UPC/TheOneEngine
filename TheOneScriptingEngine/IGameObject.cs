using System;
using System.Runtime.CompilerServices;

public class IGameObject
{
    public IGameObject(IntPtr GOptr)
    {
        containerGOptr = GOptr;
        transform = new ITransform(containerGOptr);
    }

    private IntPtr containerGOptr;

    public string name;
    public bool isActive;
    public ITransform transform;
}