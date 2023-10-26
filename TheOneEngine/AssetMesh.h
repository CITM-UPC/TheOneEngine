#ifndef __ASSET_MESH__
#define __ASSET_MESH__
#pragma once

#include "Asset.h"
#include "Texture.h"
#include "Defs.h"

#include <vector>
#include <memory>
#include <string>


class AssetMesh : public Asset
{
public:
	enum Formats { F_V3, F_V3C4, F_V3T2 };
	struct V3 { vec3f v; };
	struct V3C4 { vec3f v; vec4f c; };
	struct V3T2 { vec3f v; vec2f t; };

private:
	const enum Formats format;

	uint vertex_buffer_id;
	const uint numVerts;

	uint indexs_buffer_id;
	const uint numIndexs;

public:
	AssetMesh(uint ID, std::string path, Formats format, const void* vertex_data, uint numVerts, const uint* indexs_data = nullptr, uint numIndexs = 0);
	AssetMesh(AssetMesh&& b) noexcept;
	~AssetMesh();

	void draw();

	using Ptr = std::shared_ptr<AssetMesh>;
	static std::vector<Ptr> loadFromFile(const std::string& path);

	Texture::Ptr texture;

private:
	AssetMesh(const AssetMesh& cpy);
	AssetMesh& operator=(const AssetMesh&);
};

#endif // !__ASSET_MESH__