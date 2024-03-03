using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class IComponent
{
    protected IntPtr containerGOptr;

    public IComponent(IntPtr gameObjectPtr)
    {
        containerGOptr = gameObjectPtr;
    }
}
