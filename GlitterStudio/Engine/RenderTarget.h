#pragma once
class RenderTarget
{
private:
	unsigned int fbo;
	unsigned int rbo;
	unsigned int width;
	unsigned int height;
	unsigned int buffer;
	bool depthTesting;
	bool stencilTesting;

	void setup();
public:
	RenderTarget(unsigned int w, unsigned int h);
	~RenderTarget();

	void use();
	void clear();
	void useDepthTesting(bool val);
	void useStencilTesting(bool val);
	void setDimensions(unsigned int w, unsigned int h);
	bool hasDepthTesting() const;
	bool hasStencilTesting() const;

	unsigned int getTexture() const;
};
