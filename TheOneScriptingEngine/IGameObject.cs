﻿using System;
using System.Runtime.CompilerServices;

public class IGameObject
{
    private IntPtr containerGOptr;

    public string name;
    public bool isActive;
    public ITransform transform;

    public IGameObject()
    {
        containerGOptr = InternalCalls.GetGameObjectPtr();

        transform = new ITransform(containerGOptr);
    }

    public void Destroy()
    {
        InternalCalls.DestroyGameObject(containerGOptr);
    }

    public static IGameObject Find(string name)
    {
        IntPtr foundGOptr = InternalCalls.FindGameObject(name);

        if(foundGOptr == null)
        {
            //Insert error or something

            return new IGameObject();
        }

        IGameObject goToReturn = new IGameObject();

        goToReturn.containerGOptr = foundGOptr;
        goToReturn.transform = new ITransform(foundGOptr);

        return goToReturn;
    }
}