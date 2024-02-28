using System;
using System.Runtime.CompilerServices;

class InternalCalls
{
    //Constructors
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr GetGameObjectPtr();

    //Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ITransform GetTransform(IntPtr GOptr, out Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ITransform SetTransform(IntPtr GOptr, ref Vector3 position);

    //Debug
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PrintCSharpPtrInfo(IntPtr GOptr);
}