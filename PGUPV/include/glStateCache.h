#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "log.h"


namespace PGUPV {
	template<class GLState>
	class GLStateCapturer {
	public:
		explicit GLStateCapturer(bool captureNow = true) {
			if (captureNow)
				capture();
		}
		~GLStateCapturer() {
			if (captured)
				state.restore();
		}
		void capture() {
			state.capture();
			captured = true;
		}
		void restore() {
			if (!captured)
				ERRT("No se puede restaurar un estado no capturado");
			state.restore();
			captured = false;
		}
	protected:
		GLState state;
		bool captured;
	};

	/**
	\class BlendingState
	En el constructor captura el estado actual de OpenGL con respecto al Blending.
	Al llamar al método BlendingState::restore, se restaurará dicho estado.
	*/
	class BlendingState {
	public:
		/**
		Captura el estado de blending en el momento de crear el objeto. Captura si está
		  activo o no, el color constante de blending, la ecuación y las funciones de blending
		 */
		void capture();

		/**
		Restaura el estado capturado en el constructor
		*/
		void restore();
	private:
		GLint blending;
		glm::vec4 blendColor;
		GLint dstAlpha, dstRGB;
		GLint srcAlpha, srcRGB;
		GLint eqAlpha, eqRGB;
	};

	/**
	\class ViewportState
	Usar esta clase como atributo de la plantilla GLStateCapturer:

	GLStateCapturer<ViewportState> viewport;

	*/
	class ViewportState {
	public:
		/**
		Captura la definición del viewport en este momento.
		*/
		void capture();
		/**
		Restaura el viewport almacenado
		*/
		void restore();
	private:
		GLint view[4];
	};

	/**
	\class PolygonModeState
	En el constructor captura el modo de dibujado de los polígonos, y el método restore lo restaura
	*/
	class PolygonModeState {
	public:
		void capture();
		void restore();
	private:
		GLint polygonMode[2];
	};

	/**
	\class GenericAttribState
	En el constructor captura el valor de los atributos genéricos, y el método restore lo restaura
	*/
	class GenericAttribState {
	public:
		GenericAttribState(const std::vector<GLint> &attribIds);
		void restore();
	private:
		std::vector<std::pair<GLint, glm::vec4>> attribs;
	};

	/**
	\class ColorMasksState
	En el constructor captura el estado actual de las máscaras de color, y el método restore lo restaura
	*/
	class ColorMasksState {
	public:
		ColorMasksState();
		void restore();
	private:
		GLboolean mask[4];
	};

	/**
	\class DepthTestState
	En el constructor captura el estado actual del depth testing, y el método restore lo restaura
	*/
	class DepthTestState {
	public:
		DepthTestState();
		void restore();
	private:
		GLboolean depthTestEnabled;
	};

	/**
	\class StencilTestEnabledState
	En el constructor captura si está activado o no el Stencil, y el método restore lo restaura
	*/
	class StencilTestEnabledState {
	public:
		StencilTestEnabledState();
		void restore();
	private:
		GLboolean stencilTestEnabled;
	};

	/**
	\class StencilMaskState
	En el constructor captura si la máscara de escritura del stencil, y el método restore lo restaura
	*/
	class StencilMaskState {
	public:
		StencilMaskState();
		void restore();
	private:
		GLint stencilWriteMask;
	};
	/**
	\class CurrentProgramState
	En el constructor captura el programa instalado, y el método restore lo restaura
	*/
	class CurrentProgramState {
	public:
		CurrentProgramState();
		void restore();
	private:
		GLint prevProgram;
	};

	/**
	\class ScissorTestState
	Captura el estado del test de la tijera y lo restaura
	*/
	class ScissorTestState {
	public:
		/**
		Captura el estado del test de la tijera (on/off y la definición del área)
		*/
		void capture();
		/**
		Restaura el estado almacenado
		*/
		void restore();
	private:
		GLboolean enabled;
		GLint scissorReg[4];
	};

	/**
	\class FramebufferState
	Captura el frame buffer object actual y lo restaura
	*/
	template <GLenum Target>
	class FrameBufferObjectState {
	public:
// this warning should not appear in C++14
#ifdef _WIN32
#pragma warning (disable: 4127)
#endif
		void capture() {
			if (GL_DRAW_FRAMEBUFFER == Target)
				glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFBO);
			else if (GL_READ_FRAMEBUFFER == Target)
				glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevFBO);
			else
				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
		}
		void restore() {
			if (GL_DRAW_FRAMEBUFFER == Target)
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
			else if (GL_READ_FRAMEBUFFER == Target)
				glBindFramebuffer(GL_READ_FRAMEBUFFER, prevFBO);
			else
				glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
		}
#ifdef _WIN32
#pragma warning (default: 4127)
#endif
	private:
		GLint prevFBO;
	};

	/**
	\class ActiveTextureUnitState
	*/
	class ActiveTextureUnitState {
	public:
		void capture() {
			glGetIntegerv(GL_ACTIVE_TEXTURE, &texUnit);
		}
		void restore() {
			glActiveTexture(texUnit);
		}
	private:
		GLint texUnit;
	};

	/**
	\class TextureBindingState
	*/
	template <GLenum Target>
	class TextureBindingState {
	public:
		void capture() {
			GLenum constant;
			switch (Target) {
			case GL_TEXTURE_1D:
				constant = GL_TEXTURE_BINDING_1D;
				break;
			case GL_TEXTURE_2D:
				constant = GL_TEXTURE_BINDING_2D;
				break;
			case GL_TEXTURE_3D:
				constant = GL_TEXTURE_BINDING_3D;
				break;
			default:
				ERRT("Por completar");
			}
			glGetIntegerv(constant, &texId);
		}
		void restore() {
			glBindTexture(Target, texId);
		}
	private:
		GLint texId;
	};

  /**
\class PixelPackState
*/
  class PixelPackState {
  public:
    void capture() {
      glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
    }
    void restore() {
      glPixelStorei(GL_PACK_ALIGNMENT, alignment);
    }
  private:
    GLint alignment;
  };
};
