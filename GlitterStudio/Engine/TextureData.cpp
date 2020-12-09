#include "TextureData.h"
#include "File.h"
#include "BinaryReader.h"
#include "../Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gli/gli.hpp>

TextureData::TextureData(const std::string& path, TextureSlot slot)
{
	reload(path, slot);
}

TextureData::TextureData()
{

}

std::string TextureData::getName() const
{
	return textureName;
}

std::string TextureData::getFullname() const
{
	return fullname;
}

unsigned int TextureData::getID() const
{
	return ID;
}

TextureSlot TextureData::getSlot() const
{
	return slot;
}

int TextureData::getWidth() const
{
	return width;
}

int TextureData::getHeight() const
{
	return height;
}

void TextureData::use()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

void TextureData::dispose()
{
	glDeleteTextures(1, &ID);
}

void TextureData::reload(const std::string& path, TextureSlot slot)
{
	textureName = Glitter::File::getFileName(path);
	fullname = path;

	if (!Glitter::File::exists(path))
	{
		Logger::log(Message(MessageType::Error, "Failed to load texture " + textureName + ". File not found."));
		return;
	}

	this->slot = slot;

	read();
}

void TextureData::read()
{
	gli::texture tex = gli::load_dds(fullname);
	gli::gl gl(gli::gl::PROFILE_GL33);
	gli::gl::format const fmt = gl.translate(tex.format(), tex.swizzles());

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tex.levels() - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	for (int i = 0; i < 4; ++i)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R + i, fmt.Swizzles[i]);

	GLenum tgt = gl.translate(tex.target());
	auto target = tex.target();
	if (target != gli::TARGET_2D)
	{
		Logger::log(Message(MessageType::Warning, "Failed to load " + fullname + ". Only 2D textures are supported"));
		return;
	}

	glm::tvec3<GLsizei> const extent(tex.extent());
	width = tex.extent().x;
	height = tex.extent().y;

	for (std::size_t layer = 0; layer < tex.layers(); ++layer)
		for (std::size_t level = 0; level < tex.levels(); ++level)
			for (std::size_t face = 0; face < tex.faces(); ++face)
			{
				GLsizei const lyr = static_cast<GLsizei>(layer);
				gli::tvec3<GLsizei> extent(tex.extent(level));

				if (gli::is_compressed(tex.format()))
					glCompressedTexImage2D(tgt, level, fmt.Internal, extent.x, extent.y, 0, tex.size(level), tex.data(layer, face, level));
				else
					glTexImage2D(tgt, level, fmt.Internal, extent.x, extent.y, 0, fmt.External, fmt.Type, tex.data(layer, face, level));
			}

	glBindTexture(GL_TEXTURE_2D, 0);
}
