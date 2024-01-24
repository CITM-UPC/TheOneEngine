/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

// InputMgr.h
/// \file 
/// Contains the declaration of the Windows Input Manager class.
/// This class handles the relaying of input (Keyboard, DirectInput, XInput) to the Menu System.

#pragma once

#include <vector>

#define DIRECTINPUT_VERSION 0x0800
#include <initguid.h> // need to be included before dinput.h
#include <dinput.h>

#include "UniversalInput.h"
#include "IAkInputMgr.h"


/// Windows-Specific Input Manager class that handles all input devices and relays input
/// data to the Menu system.
class WindowsInputMgr : public IAkInputMgr
{
public:

	/// InputMgr class constructor
	WindowsInputMgr();

	/// InputMgr class destructor
	~WindowsInputMgr();

	// Set the HWND attached to DirectInput cooperative level.
	// Call this before Init.
	// Specify bIsConsoleWindow == true if and only if the HWND is not owned by the current process.
	// It is OK to not call this or pass a NULL HWND. This will not fail initialization, but you may end up with no attached devices.
	inline void SetHwnd(HWND hWnd, bool bIsConsoleWindow)
	{
		m_hWnd = hWnd;
		m_bIsConsoleWindow = bIsConsoleWindow;
	}

	virtual bool Init(AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount) override;

	virtual UniversalInput* UniversalInputAdapter() const override;

	virtual void Update() override;

	virtual void Release() override;

	virtual bool ShouldQuit() const override { return false; }
	
	AkUInt32 GetControllerOutputId(int in_iPlayerIndex) const override { return 0; }

private:

	/// Represents an input device being tracked by the Input Manager.
	struct InputObject
	{
		LPDIRECTINPUTDEVICE8 pDevice; ///< - Pointer to the device's DInput device object (NULL if Xinput).
		int iScePadHandle;			  ///< - handle for scePad (-1 if not relevant)
		int iXInputId;				 ///< - xInput id xbox (-1 if not relevant)
		int iPlayerIndex;			  ///< - The 1-based player index associated with the device.
		
		InputObject()
			: pDevice(NULL)
			, iScePadHandle(-1)
			, iXInputId(-1)
			, iPlayerIndex(-1)
		{}
	};

	/// Iterator to the Input Objects container.
	typedef std::vector<InputObject>::iterator Iterator;

	/// Adds a directinput device to the list of devices being tracked by the Input Manager.
	/// \note This function is normally called by the EnumJoysticksCallback function.
	/// \return True if there is still room to add more devices to the manager and false otherwise.
	bool AddDevice( 
		GUID in_guidInstance		///< - Instance GUID of the device to try to add
		);

	/// Retrieves input state from a DirectInput device then translates it to a
	/// Button State usable by the Universal Input class and sets it.
	void TranslateDInput(
		int in_iPlayerIndex,				///< - 1-Based Player Index of the DInput device to be polled
		LPDIRECTINPUTDEVICE8 in_pDevice		///< - DInput Device Object pointer to the device to be polled
		);

	/// Retrieves input state from a keyboard then translates it to a Button State 
	/// usable by the Universal Input class and sets it.
	void TranslateKeyboard( 
		int in_iPlayerIndex,				///< - 1-Based Player Index of the keyboard (should be 0)
		LPDIRECTINPUTDEVICE8 in_pKeyboard	///< - DInput Device Object pointer to the keyboard device
		);

	/// Retrieves input state from an XInput device (eg. Xbox 360 controller) then
	/// translates it to a Button State usable by the Universal Input class and sets it.
	void TranslateXInput( 
		int in_iPlayerIndex,			///< - 1-Based Player Index of the XInput device to be polled
		int in_XInputId					///< - XInput device id
	);

	/// Retrieves input state from an ScePad device (eg. DualSense) then
	/// translates it to a Button State usable by the Universal Input class and sets it.
	void TranslateScePadInput(
		int iPlayerIndex,				///< - 1-Based Player Index of the XInput device to be polled
		int iScePadHandle				///< - scePad handle
	);

	/// Comparison function to compare InputObjects for the c++ sort function from the algorithm library.
	/// \return True if the first parameter preceeds the second when in ascending order and false otherwise.
	static bool CompareInputObjects( 
		InputObject in_sFirst,			///< - The first Input Object
		InputObject in_sSecond			///< - The second Input Object
		);
	
	/// Callback function used by the DirectInput EnumDevices method.
	static BOOL CALLBACK EnumJoysticksCallback( 
		const DIDEVICEINSTANCE* pdidInstance,	///< - Instance of a Dinput device
		VOID* pContext							///< - Context the Callback was called in
	);

	/// The application's main window handle
	HWND m_hWnd;

	/// Pointer to the DirectInput object
	LPDIRECTINPUT8 m_pDI;

	/// Pointer to the Universal Input object to interface with the Menu System
	/// \sa UniversalInput
	UniversalInput* m_pUInput;

	/// Container to hold the Input Objects tracked by the Input Manager
	std::vector<InputObject> m_InputObjects;

	/// The maximum number of ScePad devices
	static const int MAX_SCEPAD = 4;

	// a list of scePadHandles that we have not yet added as inputObjects
	int m_pendingScePadHandles[MAX_SCEPAD];

	/// The maximum number of XInput controllers
	static const int MAX_XINPUT = 4;

	/// Thumbstick Deadzone value
	static const int DEADZONE = 15000;

	bool m_bIsConsoleWindow = false;

};