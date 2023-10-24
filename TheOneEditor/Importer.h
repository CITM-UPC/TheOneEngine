#ifndef __IMPORTER_H__
#define __IMPORTER_H__
#pragma once

#include "Defs.h"

class Asset;

class Importer
{
public:
	enum class ImporterType
	{
		MESH,
		TEXTURE,
		UNKNOWN,
	};

	struct ImportData
	{
		ImportData(const char* path)
		{
			this->path = path;
		}

		const char* path = "";
		bool dropped = false;
	};

public:
	Importer(ImporterType type);
	virtual ~Importer();

	virtual Asset* Import(ImportData& IData) const = 0;
	virtual Asset* Load(const char* path) const = 0;

	ImporterType GetType() const;

private:
	ImporterType type = ImporterType::UNKNOWN;
};

#endif // !__IMPORTER_H__