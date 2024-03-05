using System;
using System.Runtime.CompilerServices;

public class Input
{

	public enum KeyboardCode
	{
		A = 4,
		D = 7,
		S = 22,
		W = 26,
		E = 8,
		RIGHT = 79,
		LEFT = 80,
		DOWN = 81,
		UP = 82
	}

	public enum MouseButtonCode
	{
		LEFT = 1,
		MIDDLE,
		RIGHT
	}

	public enum ControllerButtonCode
	{
		START,
		BACK,
		GUIDE,
		X,
		Y,
		A,
		B,
		L1,
		R1,
		L2,
		R2,
		L3,
		R3,
		UP,
		DOWN,
		LEFT,
		RIGHT
	}

	public enum ControllerJoystickCode
	{
		LEFT, RIGHT
	}

	public static bool GetKeyboardButton(KeyboardCode key)
	{
		return InternalCalls.GetKeyboardButton((int) key);
	}
}

