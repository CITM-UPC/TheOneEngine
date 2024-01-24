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

// LabelControl.h
/// \file 
/// Contains the declaration of the Label Control type.

#pragma once

#include "Control.h"

/// The LabelControl class is a simple control that allows the user to include a "Label" in a list,
/// not to be interacted with in any meaningful way.
class LabelControl : public Control
{
public:

	/// LabelControl class contructor. Accepts a reference to the Page which contains the control.
	LabelControl( 
		Page& in_pParentPage	///< - Pointer to the Page that contains the button
		);

	/// LabelControl class destructor.
	virtual ~LabelControl();

	/// Updates the button based on the given input. If the selected value changes, the delegate function is called.
	virtual void Update(
		const UniversalGamepad& in_Gamepad	///< - The input to react to
	) 
	{}

	/// Draws the button to the screen with the desired DrawStyle.
	/// \sa DrawStyle
	virtual void Draw( 
		DrawStyle in_eDrawStyle = DrawStyle_Control		///< - The style with which to draw the control (defaults to "DrawStyle_Control")
		);

	/// Called when the control goes into focus.
	virtual void OnFocus() {}

	/// Called when the control loses focus.
	virtual void OnLoseFocus() {}

	/// Updates the text displayed on the control
	void SetText(const char* in_szText) 
	{
		m_labelText = in_szText;
	}

private:
	string m_labelText;
};
