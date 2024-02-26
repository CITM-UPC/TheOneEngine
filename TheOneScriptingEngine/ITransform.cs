using System;
using System.Runtime.CompilerServices;

public class ITransform
{
    private ulong _uid;

    public ITransform(ulong uid)
    {
        _uid = uid;
    }

    public Vector3 position
    {
        get
        {
            Vector3 outPos;
            InternalCalls.GetTransform(_uid, out outPos);
            return outPos;
        }
        set
        {
            InternalCalls.SetTransform(_uid, ref value);
        }
    }
}