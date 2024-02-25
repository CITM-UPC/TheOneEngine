using System;
using System.Runtime.CompilerServices;

class InternalCalls
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ulong GetUID(out ulong uid);
}
