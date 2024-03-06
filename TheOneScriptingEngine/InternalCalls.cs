using System;
using System.Runtime.CompilerServices;

class InternalCalls
{
    //Constructors
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr GetGameObjectPtr();

    //Input
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetKeyboardButton(int id);


    //Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetPosition(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPosition(IntPtr GOptr, ref Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetRotation(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetRotation(IntPtr GOptr, ref Vector3 rotation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Translate(IntPtr GOptr, ref Vector3 increment);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetTransformForward(IntPtr GOptr);

    //Helpers
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetAppDeltaTime();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CreateBullet(Vector3 initialPosition, Vector3 rotation);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void endBullet();
}