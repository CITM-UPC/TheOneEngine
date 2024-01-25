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
#include "stdafx.h"
#include "CommandQueue.h"

#include "RenderDevice.h"

using namespace Microsoft::WRL;

namespace d3d12
{
	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type, size_t allocatorCount)
		: m_commandListType(type)
	{
		auto device = RenderDevice::Instance().GetDevice();

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		ThrowIfFailed(device->CreateCommandQueue(&desc, PLATFORM_IDD_PPV_ARGS(m_d3d12CommandQueue.GetAddressOf())));

		m_fences.reserve(allocatorCount);
		for (uint64_t i = 0; i < allocatorCount; ++i)
			m_fences.emplace_back(device.Get());

		m_commandAllocators.reserve(allocatorCount);
		for (int i = 0; i < allocatorCount; ++i)
			m_commandAllocators.push_back(CreateCommandAllocator());
	}

	// Get an available command list from the command queue.
	ComPtr<ID3D12GraphicsCommandList> CommandQueue::CreateCommandList()
	{
		m_fences[m_index].Wait();

		auto& commandAllocator = m_commandAllocators[m_index];
		ThrowIfFailed(commandAllocator->Reset());

		// Create or reuse command list from allocator
		if (m_commandList.Get() == nullptr)
			m_commandList = CreateCommandList(commandAllocator);
		else
			ThrowIfFailed(m_commandList->Reset(commandAllocator.Get(), nullptr));

		return m_commandList;
	}

	void CommandQueue::ExecuteCommandList()
	{
		m_commandList->Close();

		ID3D12CommandList* const ppCommandLists[] = { m_commandList.Get() };
		m_d3d12CommandQueue->ExecuteCommandLists(1, ppCommandLists);
		m_fences[m_index].Signal(m_d3d12CommandQueue.Get());

		m_index = (m_index + 1) % (uint32_t)m_fences.size();
	}

	ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
	{
		auto device = RenderDevice::Instance().GetDevice();

		ComPtr<ID3D12CommandAllocator> commandAllocator;
		ThrowIfFailed(device->CreateCommandAllocator(m_commandListType, PLATFORM_IDD_PPV_ARGS(commandAllocator.GetAddressOf())));
		return commandAllocator;
	}

	ComPtr<ID3D12GraphicsCommandList> CommandQueue::CreateCommandList(ComPtr<ID3D12CommandAllocator> allocator)
	{
		auto device = RenderDevice::Instance().GetDevice();

		ComPtr<ID3D12GraphicsCommandList> commandList;
		ThrowIfFailed(device->CreateCommandList(0, m_commandListType, allocator.Get(), nullptr, PLATFORM_IDD_PPV_ARGS(commandList.GetAddressOf())));
		return commandList;
	}
}
