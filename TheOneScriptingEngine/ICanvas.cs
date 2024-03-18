using System;

public class ICanvas : IComponent
{
    public ICanvas(IntPtr GOptr) : base(GOptr) { }

    public int GetSelection()
    {
        return InternalCalls.GetSelectiedButton(containerGOptr);
    }

    public void MoveSelection(int direction)
    {
        InternalCalls.MoveSelectedButton(containerGOptr, direction);
    }
}