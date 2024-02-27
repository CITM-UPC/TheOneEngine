using System;

public class Class1 : MonoBehaviour
{
    Class1()
    {
        Console.WriteLine("Called constructor of Class1 with GameObject name: " + attachedGameObject.name + " from C#.");
    }

    public float MyPublicFloatVar = 5.0f;

    public void PrintFloatVar()
    {
        Console.WriteLine("MyPublicFloatVar = {0:F}", MyPublicFloatVar);
    }

    private void IncrementFloatVar(float value)
    {
        MyPublicFloatVar += value;
    }

}
