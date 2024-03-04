using System;
using System.Runtime.CompilerServices;

public class ITransform : IComponent
{
    public Vector3 position
    {
        get
        {
            return InternalCalls.GetPosition(containerGOptr);
        }
        set
        {
            InternalCalls.SetPosition(containerGOptr, ref value);
        }
    }

    public Vector3 forward
    {
        get
        {
            return InternalCalls.GetTransformForward(containerGOptr);
        }
    }

    public ITransform(IntPtr GOptr) : base(GOptr) { }

    public void Translate(Vector3 increment)
    {
        InternalCalls.Translate(containerGOptr, ref increment);
    }
}