using System;
using System.Runtime.CompilerServices;

public class IGameObject
{
    public IGameObject(ulong uuid)
    {
        _uuid = uuid;
        transform = new ITransform(_uuid);
    }

    private ulong _uuid;

    public string name;
    public bool isActive;
    public ITransform transform;
}