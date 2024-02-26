using System;
using System.Runtime.CompilerServices;

public struct Vector3
{
    float x;
    float y;
    float z;

    float Magnitude()
    {
        return (float)Math.Sqrt(x * x + y * y + z * z);
    }

    Vector3 Normalize()
    {
        return new Vector3(x, y, z) / Magnitude();
    }

    public static Vector3 operator /(Vector3 a, float b)
    {
        return new Vector3(a.x / b, a.y / b, a.z / b);
    }

    Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}