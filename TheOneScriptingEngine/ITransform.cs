using System;
using System.Runtime.CompilerServices;

public class ITransform : IComponent
{
    public ITransform(IntPtr GOptr) : base(GOptr) { }

    public Vector3 position
    {
        get
        {
            Vector3 outPos;
            InternalCalls.GetTransform(containerGOptr, out outPos);
            return outPos;
        }
        set
        {
            InternalCalls.SetTransform(containerGOptr, ref value);
        }
    }

    public void Move(Vector3 increment)
    {
        Vector3 targetPos = position + increment;
        InternalCalls.SetTransform(containerGOptr, ref targetPos);
    }
}