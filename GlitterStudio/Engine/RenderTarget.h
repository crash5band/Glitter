#pragma once
#include "MathGens.h"

namespace Glitter
{
	namespace Engine
	{
		class RenderTarget
		{
		private:
			unsigned int fbo;
			unsigned int ifbo;
			unsigned int irbo;
			unsigned int rbo;
			unsigned int width;
			unsigned int height;
			unsigned int result;
			unsigned int buffer;
			int numSamples;
			int anisotropicFiltering;
			Color clearColor;

			bool depthTesting;
			bool stencilTesting;

			void createTexture(int texID);
			void createMultiSampleTexture(int texID);
			void setup();
		public:
			RenderTarget(unsigned int w, unsigned int h, int samples);
			~RenderTarget();

			void use();
			void end();
			void clear();
			void dispose();
			void useDepthTesting(bool val);
			void useStencilTesting(bool val);
			void resize(unsigned int w, unsigned int h);
			void setClearColor(const Glitter::Color& color);
			void setSampleCount(int samples);
			void setFiltering(int count);

			bool hasDepthTesting() const;
			bool hasStencilTesting() const;

			inline unsigned int getWidth() const { return width; }
			inline unsigned int getHeight() const { return height; }
			inline int getSampleCount() const { return numSamples; }
			inline Color getClearColor() const { return clearColor; }

			unsigned int getTexture() const;
		};

	}
}