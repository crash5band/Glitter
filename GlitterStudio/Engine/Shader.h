#pragma once

#include <glad/glad.h>
#include "..\Dependencies\DirectXMath-master\Inc\DirectXMath.h"
#include <string>

class Shader
{
private:
	unsigned int ID;
	unsigned int uloc;
	std::string name;

	void compile(const char* vSource, const char* fSource);

public:	
	Shader(const std::string& name, const char* v, const char* f);
	~Shader();

	std::string getName() const;
	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, DirectX::XMVECTOR v) const;
	void setVec3(const std::string& name, DirectX::XMVECTOR v) const;
	void setVec4(const std::string& name, DirectX::XMVECTOR v) const;
	void setMatrix4(const std::string& name, DirectX::XMMATRIX m) const;
};