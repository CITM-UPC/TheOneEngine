using System;
using System.Runtime.CompilerServices;

public class IGameObject
{
    public IGameObject()
    {
        containerGOptr = InternalCalls.GetGameObjectPtr();
        InternalCalls.PrintCSharpPtrInfo(containerGOptr);

        transform = new ITransform(containerGOptr);
    }

    private IntPtr containerGOptr;

    public string name;
    public bool isActive;
    public ITransform transform;
}