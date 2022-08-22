#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace PGUPV {
	class Model;
	class Axes;

	class Camera {
	public:
		Camera() : viewMatrix(1.0f), projMatrix(1.0f), frustumColor(0.2f, 0.2f, 0.2f, 1.0f), 
			recomputeFrustum(true), axesScale{ 1.0f } {};
		~Camera();
		/**
		Devuelve la matriz de la vista actual
		*/
		glm::mat4 getViewMatrix() const { return viewMatrix; };
		/**
		Devuelve la matriz de proyección.
		*/
		glm::mat4 getProjMatrix() const { return projMatrix; };
		/**
		Devuelve la dirección de la vista actual en el S.C. del mundo
		*/
		glm::vec3 getViewDirection() const;
		/**
		Devuelve el vector up actual en el S.C. del mundo
		*/
		glm::vec3 getUpDirection() const;
		/**
		Devuelve la dirección del eje X local a la cámara (right) en el S.C. del mundo
		*/
		glm::vec3 getRightDirection() const;
		/**
		Devuelve la posición actual de la cámara en el S.C. del mundo
		*/
		glm::vec3 getCameraPos() const;
		/**
		Devuelve el FOV de la cámara
		*/
		float getFOV() const;
		/**
		Devuelve el aspect ratio de la cámara
		*/
		float getAspectRatio() const;
		/**
		Devuelve la distancia near de la cámara
		*/
		float getNear() const;
		/**
		Devuelve la distancia far de la cámara
		*/
		float getFar() const;
		/**
		Establece una nueva matriz de la vista
		*/
		void setViewMatrix(const glm::mat4 &viewMatrix);
		/**
		Establece una nueva matriz de proyección
		*/
		void setProjMatrix(const glm::mat4 &projMatrix);
		/**
		Dibuja el volumen de la cámara
		*/
		void render() const;
		/**
		Establece el color con el que se dibujará el frustum
		*/
		void setFrustumColor(const glm::vec4 &color);

		/** 
		Dada una matriz de proyección devuelve los parámetros del volumen de la cámara
		\warning En la descomposición se pierde algo de precisión. Si la precisión es importante, guarda
		los parámetros que definieron la matriz de proyección.
		*/
		enum class CameraType {
			Unknown, Perspective, Orthographic
		};
		struct ViewVolume {
			float left, right, bottom, top, near, far;
			CameraType type;
		};
		static ViewVolume decomposeProjMatrix(const glm::mat4& proj);
	protected:
		glm::mat4 viewMatrix, projMatrix;
		glm::vec4 frustumColor;
		mutable std::shared_ptr<Model> frustum;
		mutable bool recomputeFrustum;
		mutable float axesScale;
		static std::unique_ptr<Axes> axes;
		void updateVerticesInFrustum() const;
	};
};
