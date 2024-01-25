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

#include "Shader_Tex2DSimpleCommon.hlsl"

struct VS_INPUT
{
	float2 pos 		: POS2D;
	float2 texcoord : TEXCOORD0;
};

[RootSignature(TexRS)]
VS_OUTPUT VsMain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = float4(input.pos * float2(2, -2) + float2(-1, 1), 0.5, 1);
	output.texcoord = input.texcoord;
	return output;
}
