using System;
using System.Runtime.CompilerServices;

class InternalCalls
{
    //Constructors
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ulong GetUID(out ulong uid);

    //Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ITransform GetTransform(ulong uid, out Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ITransform SetTransform(ulong uid, ref Vector3 position);
}
