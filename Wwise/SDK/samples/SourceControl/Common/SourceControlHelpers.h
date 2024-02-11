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
////////////////////////////////////////////////////////////////////////
// SourceControlHelpers.h
//
// SourceControlHelpers header file
//
// Author:  brodrigue
// Version: 1.0
//
////////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/Wwise/SourceControl/ISourceControl.h>
#include <AK/Wwise/SourceControl/SourceControlContainers.h>

#include <variant>
#include <vector>

using StringVector = std::vector<CString>;

// Read-only polymorphic view that allows to iterate either on a IAkList or a StringVector
// Note that the underlying container must be contiguous in memory
// Since new IAkList can't be instantiated from within the plugins, another type must be used
// (e.g. StringVector) and this view allows to unify code that expects one or the other as input
class StringListView
{
	using StringType = LPCWSTR;
	using VariantType = std::variant<const StringType*, const CString*>;

	struct Iterator
	{
		Iterator(const VariantType& in_data)
			: m_data(in_data)
		{
		}
		Iterator& operator++()
		{
			std::visit([](const auto*& in_data) {
				++in_data;
			}, m_data);
			return *this;
		}
		StringType operator*() const
		{
			return std::visit([](const auto* in_data) -> StringType {
				return *in_data;
			}, m_data);
		}
		friend bool operator==(const Iterator& in_it1, const Iterator& in_it2)
		{
			return in_it1.m_data == in_it2.m_data;
		}
		friend bool operator!=(const Iterator& in_it1, const Iterator& in_it2)
		{
			return !(in_it1 == in_it2);
		}

	private:
		VariantType m_data;
	};

public:
	template<class Arg_Type>
	StringListView(const AK::Wwise::SourceControlContainers::IAkList<StringType, Arg_Type>& in_list)
		: m_data(!in_list.IsEmpty() ? &in_list.GetHead() : nullptr)
		, m_size(in_list.GetSize())
	{
	}

	StringListView(const StringVector& in_list)
		: m_data(!in_list.empty() ? &in_list[0] : nullptr)
		, m_size(in_list.size())
	{
	}

	StringType operator[](size_t i) const
	{
		return std::visit([i](const auto* in_data) -> StringType {
			return in_data[i];
		}, m_data);
	}

	size_t size() const
	{
		return m_size;
	}

	bool empty() const
	{
		return m_size != 0;
	}

	Iterator begin() const
	{
		return m_data;
	}

	Iterator end() const
	{
		return std::visit([this](const auto* in_data) -> Iterator {
			return Iterator(in_data + m_size);
		}, m_data);
	}

private:
	const VariantType m_data;
	const size_t m_size;
};

namespace SourceControlHelpers
{
	bool CanProceedWithMove( 
		const StringListView& in_rFilenameList, 
		const CString& in_csDestinationDir,
		std::vector<CString>& out_rNewPaths,
		AK::Wwise::ISourceControlUtilities* in_pUtilities,
		LPCTSTR in_pszErrorAlreadyExist,
		LPCTSTR in_pszErrorNameConflict );

	CString GetLastErrorString();

	bool WaitWithPaintLoop(HANDLE hEvent);

	bool IsMainThread();
};
