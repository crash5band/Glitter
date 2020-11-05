#include "RenderTarget.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

RenderTarget::RenderTarget(unsigned int w, unsigned int h) :
	width{ w }, height{ h }, depthTesting{ true }, stencilTesting{ false }
{
	setup();
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &buffer);
}

void RenderTarget::clear()
{
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	GLbitfield clearBits = GL_COLOR_BUFFER_BIT;
	if (depthTesting) clearBits |= GL_DEPTH_BUFFER_BIT;
	if (stencilTesting) clearBits |= GL_STENCIL_BUFFER_BIT;
	glClear(clearBits);
}

void RenderTarget::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);
}

void RenderTarget::useDepthTesting(bool val)
{
	depthTesting = val;
}

void RenderTarget::useStencilTesting(bool val)
{
	stencilTesting = val;
}

void RenderTarget::setDimensions(unsigned int w, unsigned int h)
{
	width = w;
	height = h;
}

bool RenderTarget::hasDepthTesting() const
{
	return depthTesting;
}

bool RenderTarget::hasStencilTesting() const
{
	return stencilTesting;
}

unsigned int RenderTarget::getTexture() const
{
	return buffer;
}

void RenderTarget::setup()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("RenderTarget::setup() ERROR: Incomplete framebuffer");

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}