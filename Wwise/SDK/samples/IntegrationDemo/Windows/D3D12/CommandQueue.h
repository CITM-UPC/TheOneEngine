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
#pragma once

#include "Utilities.h"

#include <array>
#include <vector>

namespace d3d12
{
	// Synchronized command queue using command allocator and fence ring buffers.
	// Only supports creating a single command list at a time. The command list is reused after execution.
	class CommandQueue
	{
	private:
		class Fence
		{
		public:
			Fence(ID3D12Device* device)
				: event(::CreateEvent(NULL, FALSE, FALSE, NULL))
			{
				assert(event && "Failed to create fence event handle.");
				ThrowIfFailed(device->CreateFence(value, D3D12_FENCE_FLAG_NONE, PLATFORM_IDD_PPV_ARGS(d3d12Fence.GetAddressOf())));
			}

			bool IsComplete() const
			{
				return d3d12Fence->GetCompletedValue() >= value;
			}

			void Signal(ID3D12CommandQueue* commandQueue)
			{
				commandQueue->Signal(d3d12Fence.Get(), ++value);
			}

			void Wait() const
			{
				if (!IsComplete())
				{
					d3d12Fence->SetEventOnCompletion(value, event);
					::WaitForSingleObject(event, DWORD_MAX);
				}
			}

			void Flush(ID3D12CommandQueue* commandQueue)
			{
				Signal(commandQueue);
				Wait();
			}

		private:
			Microsoft::WRL::ComPtr<ID3D12Fence> d3d12Fence{};
			HANDLE event{};
			uint64_t value = 0;
		};

	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type, size_t allocatorCount);

		// Create a command list from the current command allocator.
		// Will wait if previous command list is still running.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList();

		// Execute the last command list created with CreateCommandList.
		void ExecuteCommandList();

		ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_d3d12CommandQueue.Get(); }

		uint32_t GetCommandListIndex() const { return m_index; }

		void Flush()
		{
			for (auto& fence : m_fences)
			{
				fence.Flush(m_d3d12CommandQueue.Get());
			}
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

	private:
		D3D12_COMMAND_LIST_TYPE m_commandListType;

		// Keep one fence per allocator and cycle through them
		uint32_t m_index = 0;
		std::vector<Fence> m_fences;
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_commandAllocators;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue{};
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	};
}
