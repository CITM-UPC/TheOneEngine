#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once

#include <string>
#include <memory>

class Texture
{
private:
	unsigned int _id;

public:
	using Ptr = std::shared_ptr<Texture>;
	explicit Texture(const std::string& path);
	Texture(Texture&& tex) noexcept;
	void bind() const;
	~Texture();

private:
	Texture(const Texture&);
	Texture operator=(const Texture&);

};

#endif // !__TEXTURE_H__