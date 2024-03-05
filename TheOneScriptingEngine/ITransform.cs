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

    public Vector3 rotation
    {
        get
        {
            return InternalCalls.GetRotation(containerGOptr);
        }
        set
        {
            InternalCalls.SetRotation(containerGOptr, ref value);
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
        //This implementation is temporary, engine Transform.Translate is not working properly.

        Vector3 finalPos = position + increment;
        InternalCalls.Translate(containerGOptr, ref finalPos);
    }
}