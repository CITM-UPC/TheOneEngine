﻿using System;
using System.Runtime.CompilerServices;

public struct Vector3
{
    public float x { get; }
    public float y { get; }
    public float z { get; }

    public static Vector3 zero      { get { return new Vector3(0, 0, 0); } }
    public static Vector3 right     { get { return new Vector3(1.0f, 0, 0); } }
    public static Vector3 up        { get { return new Vector3(0, 1.0f, 0); } }
    public static Vector3 forward   { get { return new Vector3(0, 0, 1.0f); } }

    public Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public static Vector3 operator +(Vector3 a, Vector3 b)
    {
        return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    public static Vector3 operator *(Vector3 a, float b)
    {
        return new Vector3(a.x * b, a.y * b, a.z * b);
    }

    public static Vector3 operator /(Vector3 a, float b)
    {
        return new Vector3(a.x / b, a.y / b, a.z / b);
    }

    float Magnitude()
    {
        return (float)Math.Sqrt(x * x + y * y + z * z);
    }

    Vector3 Normalize()
    {
        return new Vector3(x, y, z) / Magnitude();
    }
}