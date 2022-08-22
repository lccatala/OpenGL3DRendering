
#ifndef _GL_MATRICES_H
#define _GL_MATRICES_H 2011

#include <GL/glew.h>
#include "matrixStack.h"
#include "uniformBufferObject.h"

/**
\class GLMatrices
Esta clase mantiene las matrices de transformación de vértice tradicional
de OpenGL (ModelView, Projection, Normal, etc), además de Model y View.

Para insertar automáticamente la definición del bloque de uniforms en tu
shader puedes introducir la siguiente línea en el lugar donde quieres que
aparezca:

$GLMatrices

Dicha línea se sustituirá por el siguiente bloque antes de compilar el shader:

layout (std140) uniform GLMatrices {
mat4 modelMatrix;
mat4 viewMatrix;
mat4 projMatrix;
mat4 modelviewMatrix;
mat4 modelviewprojMatrix;
mat3 normalMatrix;
};

*/

namespace PGUPV {
  class GLMatrices : public UniformBufferObject, public std::enable_shared_from_this<GLMatrices> {
  public:
    // ¡Cuidado! Las matrices ModelView, ModelViewProjection y Normal se 
    // actualizan automáticamente a partir de las demás: NO modificarlas
    enum Matrix {
      MODEL_MATRIX, VIEW_MATRIX, PROJ_MATRIX, MODELVIEW_MATRIX,
      MODELVIEWPROJ_MATRIX, NORMAL_MATRIX
    };
    static std::shared_ptr<GLMatrices> build();
    const std::string &getBlockName() const override;
    const Strings &getDefinition() const override;

	static const std::string blockName;
	static const Strings definition;
    // Devuelve el tamaño del tipo GLMatrices. Cuidado! No usar sizeof(GLMatrices)!
    static uint size();
    void loadIdentity(Matrix mat);
    void pushMatrix(Matrix mat);
    void popMatrix(Matrix mat);
    void translate(Matrix mat, float x, float y, float z);
    void translate(Matrix mat, const glm::vec3 &t);
    void rotate(Matrix mat, float radians, float axis_x, float axis_y, float axis_z);
    void rotate(Matrix mat, float radians, const glm::vec3 &axis);
    void scale(Matrix mat, float s);
    void scale(Matrix mat, float sx, float sy, float sz);
    void scale(Matrix mat, const glm::vec3 &s);
    void multMatrix(Matrix mat, const glm::mat4 &m);
    void setMatrix(Matrix mat, const glm::mat4 &m);
    const glm::mat4 &getMatrix(Matrix mat) const;
    const glm::mat3 getNormalMatrix() const;
    void reset();
  private:
    GLMatrices();
    GLMatrices(const GLMatrices&);

    void writeMatrix(Matrix mat);
    MatrixStack mats[PROJ_MATRIX + 1];
    struct {
      glm::mat4 modelview;
      glm::mat4 modelviewprojection;
      glm::mat3x4 normal; // Debido a como se almacenan las matrices con la directiva std140
    } derivedMatrices;

    friend std::ostream& operator<<(std::ostream &os, const GLMatrices& m);
  };
  std::ostream& operator<<(std::ostream &os, const GLMatrices& m);
};

#endif
