#include "RenderTarget.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

namespace Glitter
{
	namespace Engine
	{
		RenderTarget::RenderTarget(unsigned int w, unsigned int h, int samples) :
			width{ w }, height{ h }, numSamples{ samples }, depthTesting{ true }, stencilTesting{ false }
		{
			anisotropicFiltering = 16;
			clearColor = Color(0.1, 0.1, 0.1, 1.0);
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
			glClearColor(clearColor.r, clearColor.g, clearColor.b, 0.0);

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

		void RenderTarget::end()
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ifbo);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glDisable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void RenderTarget::dispose()
		{
			glDeleteFramebuffers(1, &fbo);
			glDeleteFramebuffers(1, &ifbo);
			glDeleteRenderbuffers(1, &rbo);
			glDeleteRenderbuffers(1, &irbo);
			glDeleteTextures(1, &buffer);
			glDeleteTextures(1, &result);
		}

		void RenderTarget::useDepthTesting(bool val)
		{
			depthTesting = val;
		}

		void RenderTarget::useStencilTesting(bool val)
		{
			stencilTesting = val;
		}

		void RenderTarget::setClearColor(const Glitter::Color& color)
		{
			clearColor = color;
		}

		void RenderTarget::setSampleCount(int samples)
		{
			if (samples > 0 && samples <= 16)
				numSamples = samples;

			createMultiSampleTexture(buffer);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH24_STENCIL8, width, height);

			createTexture(result);
		}

		void RenderTarget::setFiltering(int count)
		{
			if (count > 0 && count <= 16)
				anisotropicFiltering = count;

			createMultiSampleTexture(buffer);
			createTexture(result);
		}

		void RenderTarget::resize(unsigned int w, unsigned int h)
		{
			if (width == w && height == h)
				return;

			width = w;
			height = h;

			createMultiSampleTexture(buffer);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH24_STENCIL8, width, height);

			createTexture(result);
			glBindRenderbuffer(GL_RENDERBUFFER, irbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
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
			return result;
		}

		void RenderTarget::createTexture(int tex)
		{
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		}

		void RenderTarget::createMultiSampleTexture(int tex)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB, width, height, GL_TRUE);
		}

		void RenderTarget::setup()
		{
			// setup mutlisampled framebuffer
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glGenTextures(1, &buffer);
			createMultiSampleTexture(buffer);

			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, buffer, 0);

			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				printf("RenderTarget::setup() ERROR: Incomplete framebuffer");

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// setup result framebuffer
			glGenFramebuffers(1, &ifbo);
			glBindFramebuffer(GL_FRAMEBUFFER, ifbo);
			glGenTextures(1, &result);
			createTexture(result);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result, 0);

			glGenRenderbuffers(1, &irbo);
			glBindRenderbuffer(GL_RENDERBUFFER, irbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, irbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				printf("RenderTarget::setup() ERROR: Incomplete framebuffer");

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
	}
}
