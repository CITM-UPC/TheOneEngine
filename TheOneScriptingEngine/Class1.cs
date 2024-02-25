using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Class1
{
    ulong uid;

    Class1()
    {
        InternalCalls.GetUID(out uid);
        Console.WriteLine("Called constructor of Class1 with UID " + uid + " from C#.");
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
