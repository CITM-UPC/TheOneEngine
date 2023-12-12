#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include <string>
#include <memory>

class GameObject;

class Texture : public Component
{
private:
	unsigned int _id;

public:
	using Ptr = std::shared_ptr<Texture>;
	Texture(std::shared_ptr<GameObject> containerGO);

	explicit Texture(std::shared_ptr<GameObject> containerGO, const std::string& path);
	Texture(Texture&& tex) noexcept;
	Texture();
	void bind() const;
	~Texture();

private:
	Texture(const Texture&);
	Texture operator=(const Texture&);

public:
	std::string path;
	uint width, height;
	bool active;
};

#endif // !__TEXTURE_H__