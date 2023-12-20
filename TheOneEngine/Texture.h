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

	//Texture(std::shared_ptr<GameObject> containerGO);

	explicit Texture(const std::string& path, std::shared_ptr<GameObject> containerGO = nullptr);
	//Texture(Texture&& tex) noexcept;
	void bind() const;
	~Texture();

	//void SetTexture(Ptr texture);


private:
	/*Texture(const Texture&);
	Texture operator=(const Texture&);*/

public:
	std::string path;
	uint width, height;
	bool active;

	//using Ptr = std::shared_ptr<Texture>;
	//Texture::Ptr _texture = nullptr;
};

#endif // !__TEXTURE_H__