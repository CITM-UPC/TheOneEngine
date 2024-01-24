#pragma once

#include <AK/SoundEngine/Common/AkSoundEngine.h>

class UniversalInput;

class IAkInputMgr
{
public:
	virtual ~IAkInputMgr() {}
	
	/// Initializes the Input Manager object.
	/// \return True if the Input Manager has been properly initialized and false otherwise.
	virtual bool Init(
		AkOSChar* in_szErrorBuffer,				///< - Buffer where error details will be written (if the function returns false)
		unsigned int in_unErrorBufferCharCount	///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
	) = 0;

	/// Ends the Input Manager class and releases all used resources.
	virtual void Release() = 0;

	/// Accessor method for the UniversalInput object.
	/// \return A pointer to the UniversalInput object being used by the Input Manager.
	virtual UniversalInput* UniversalInputAdapter() const = 0;

	/// Updates the input from all devices and relays the new input states to the Menu system.
	/// \remark This method should be called every frame before the Menu system's Update call.
	virtual void Update() = 0;

	/// Used by main loops to determine whether the user has requested application quit
	virtual bool ShouldQuit() const = 0;

	// Used by DemoMotion to send sound effects to a specific controller speaker
	virtual AkUInt32 GetControllerOutputId(int iPlayerIndex) const = 0;
};
