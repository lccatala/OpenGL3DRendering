#ifndef _DRAW_COMMAND_H
#define _DRAW_COMMAND_H

#include <vector>
#include <GL/glew.h>
#include "common.h"

/** \file drawCommand.h

\author Paco Abad

*/

namespace PGUPV {

  struct TriangleIndices;

  /**
  \class DrawCommand

  Clase virtual que representa una orden de dibujado de OpenGL (glDrawArrays, glDrawElements, etc)
  Los comandos de verdad serán subclases que implementarán la función render.
  ¡Cuidado! Si dibujas GL_PATCHES, recuerda establecer el número de vértices por patch con una llamada a
  setVerticesPerPatch.
  */
  class DrawCommand {
  public:
    explicit DrawCommand(GLenum mode) : mode(mode), verticesPerPatch(0), restartPrimitive(false), restartIndex(0xffffffff) {};
    virtual ~DrawCommand() {};
    void render();
    virtual void renderFunc() = 0;
    void setVerticesPerPatch(GLint nvertices) { verticesPerPatch = nvertices; };
    GLint getVerticesPerPatch() const { return verticesPerPatch; };
    /**
    Activa/desactiva el reinicio de primitivas para este comando de dibujo
    */
    void setPrimitiveRestart(bool restart = true);
    /**
    Establece el valor del índice de reinicio
    \param rindex al encontrar este índice, OpenGL iniciará una nueva primitiva
    */
    void setRestartIndex(uint rindex);
    /**
    Una lista con los índices de cada triángulo
    \param indicesBuffer El puntero a la zona de memoria que contiene los índices
    \warning Si la primitiva no es de tipo triángulo, se lanzará una excepción
    \warning Si la primitiva usa restart, puede que alguno de los triángulos devueltos contenga
    el índice de restart (deberías ignorar ese triángulo)
    */
    virtual std::vector<TriangleIndices> getTrianglesIndices(void *indicesBuffer);

	/**
	\return el tipo de primitiva OpenGL que se dibujará (GL_TRIANGLES, GL_LINE_LOOP...)
	*/
	GLenum getGLPrimitiveType() const { return mode; }
  protected:
    GLenum mode;
    GLint verticesPerPatch;
    bool restartPrimitive;
    GLuint restartIndex;
  };

  struct TriangleIndices {
    TriangleIndices() : TriangleIndices(0, 0, 0) {};
    TriangleIndices(uint first, uint second, uint third) {
      idx[0] = first; idx[1] = second; idx[2] = third;
    }
    uint idx[3];
  };

  /**
  \class DrawArrays

  Clase envoltorio de glDrawArrays
  */
  class DrawArrays : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glDrawArrays
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param first: índice del primer vértice a dibujar
      \param count: cuántos vértices dibujar
      */
     DrawArrays(GLenum mode, GLint first, GLsizei count) : 
        DrawCommand(mode), first(first), count(count){};
    virtual void renderFunc() override {
      glDrawArrays(mode, first, count);
    }
    std::vector<TriangleIndices> getTrianglesIndices(void *indicesBuffer) override;
  private:
    GLint first; GLsizei count;
  };

  /**
  \class DrawElements

  Clase envoltorio de glDrawElements
  */
  class DrawElements : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glDrawElements
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param count: número de índices a dibujar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param offset: posición (en bytes desde el comienzo del buffer vinculado
      a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      */
    DrawElements(GLenum mode, GLsizei count, GLenum type, const void *offset) :
      DrawCommand(mode), count(count), type(type), offset(offset) {};
    void renderFunc() override {
      glDrawElements(mode, count, type, offset);
    }
    std::vector<TriangleIndices> getTrianglesIndices(void *indicesBuffer) override;
  private:
    GLsizei count; GLenum type; const void *offset;
  };

  /**
  \class DrawElementsBaseVertex

  Clase envoltorio de glDrawElementsBaseVertex

  */
  class DrawElementsBaseVertex : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glDrawElementsBaseVertex
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param count: número de índices a dibujar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param offset: posición (en bytes desde el comienzo del buffer vinculado
      a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      \param basevertex: desplazamiento a sumar a cada índice
      */
    DrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLvoid  *offset,
      GLint basevertex) :
      DrawCommand(mode), count(count), type(type), offset(offset), basevertex(basevertex) {};
    virtual void renderFunc() override {
      glDrawElementsBaseVertex(mode, count, type, offset, basevertex);
    }
  private:
    GLsizei count; GLenum type; GLvoid *offset; GLint basevertex;
  };


  /**
  \class DrawRangeElements

  Clase envoltorio de glDrawRangeElements

  */
  class DrawRangeElements : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glDrawRangeElements
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param start: índice menor a dibujar
      \param end: índice mayor a dibujar
      \param count: número de vértices a dibujar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param offset: posición (en bytes desde el comienzo del buffer vinculado
      a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      */
    DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type,
      const void *offset) : DrawCommand(mode), start(start), end(end), count(count),
      type(type), offset(offset) {};
    virtual void renderFunc() override {
      glDrawRangeElements(mode, start, end, count, type, offset);
    }
  private:
    GLuint start; GLuint end; GLsizei count; GLenum type; const void *offset;
  };

  /**
  \class DrawRangeElementsBaseVertex

  Clase envoltorio de glDrawRangeElementsBaseVertex

  */
  class DrawRangeElementsBaseVertex : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glDrawRangeElementsBaseVertex
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param start: índice menor a dibujar
      \param end: índice mayor a dibujar
      \param count: número de vértices a dibujar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param offset: posición (en bytes desde el comienzo del buffer vinculado
      a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      \param basevertex: desplazamiento a sumar a cada índice
      */
    DrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type,
      GLvoid  *offset, GLint basevertex) : DrawCommand(mode), start(start), end(end), count(count),
      type(type), offset(offset), basevertex(basevertex) {};
    virtual void renderFunc() override {
      glDrawRangeElementsBaseVertex(mode, start, end, count, type, offset, basevertex);
    }
  private:
    GLuint start; GLuint end; GLsizei count; GLenum type; GLvoid  *offset;
    GLint basevertex;
  };

  /**
  \class MultiDrawArrays

  Clase envoltorio de glMultiDrawArrays

  */
  class MultiDrawArrays : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glMultiDrawArrays
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param first: array con primcount elementos, donde cada uno indica el primer vértice
      a dibujar
      \param count: array con primcount elementos, donde cada uno indica cuántos vértices utilizar
      \param primcount: número de primitivas a dibujar

      \warning Este objeto hace una copia de los arrays pasados. Puedes liberar los originales
      */
    MultiDrawArrays(GLenum mode, const GLint *first, const GLint *count, GLsizei primcount);
    virtual void renderFunc() override {
      glMultiDrawArrays(mode, &first[0], &count[0], primcount);
    }
  private:
    GLsizei primcount;
    std::vector<GLint> first;
    std::vector<GLint> count;
  };

  /**
  \class MultiDrawElements

  Clase envoltorio de glMultiDrawElements
  Este objeto hace una copia de los arrays pasados. Puedes liberar los originales

  */
  class MultiDrawElements : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glMultiDrawElements
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param count: array con primcount elementos, donde cada uno indica cuántos vértices utilizar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param indices: array con primcount elementos, donde cada uno es la posición (en bytes desde el
      comienzo del buffer vinculado a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      \param primcount: número de primitivas a dibujar

      \warning Este objeto hace una copia de los arrays pasados. Puedes liberar los originales
      */
    MultiDrawElements(GLenum mode, const GLint *count, GLenum type, const void * const *indices, GLsizei primcount);
    virtual void renderFunc() override {
      glMultiDrawElements(mode, &count[0], type, &indices[0], primcount);
    }
  private:

    std::vector<GLint> count;
    GLenum type;
    std::vector<const void *> indices;
    GLsizei primcount;
  };


  /**
  \class MultiDrawElementsBaseVertex

  Clase envoltorio de glMultiDrawElementsBaseVertex
  Este objeto hace una copia de los arrays pasados. Puedes liberar los originales

  */
  class MultiDrawElementsBaseVertex : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glMultiDrawElementsBaseVertex
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param count: array con primcount elementos, donde cada uno indica cuántos vértices utilizar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param indices: array con primcount elementos, donde cada uno es la posición (en bytes desde el
      comienzo del buffer vinculado a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      \param primcount: número de primitivas a dibujar
      \param baseVertex: array de primcount elementos, donde cada indica una base a sumar a cada primitiva
      (equivalente al último parámetro de glDrawElementsBaseVertex)

      \warning Este objeto hace una copia de los arrays pasados. Puedes liberar los originales
      */
    MultiDrawElementsBaseVertex(GLenum mode, const GLint *count, GLenum type, void **indices,
      GLsizei primcount, const GLint *baseVertex);
    virtual void renderFunc() override {
      glMultiDrawElementsBaseVertex(mode, &count[0], type, &indices[0], primcount, &baseVertex[0]);
    }
  private:

    std::vector<GLint> count;
    GLenum type;
    std::vector<void *> indices;
    GLsizei primcount;
    std::vector<GLint> baseVertex;
  };

  /**
  \class DrawArraysInstanced

  Clase envoltorio de glDrawArraysInstanced
  */
  class DrawArraysInstanced : public DrawCommand {
  public:
    DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount) :
      DrawCommand(mode), first(first), count(count), primcount(primcount) {};
    virtual void renderFunc() override {
      glDrawArraysInstanced(mode, first, count, primcount);
    }
  private:
    GLint first; GLsizei count, primcount;
  };

  /**
  \class DrawElementsInstanced

  Clase envoltorio de glDrawElementsInstanced
  */
  class DrawElementsInstanced : public DrawCommand {
  public:
    /**
      Necesita la información para invocar a glDrawElementsInstanced
      \param mode: tipo de primitivas (GL_TRIANGLE_STRIP, GL_POINTS...)
      \param count: número de vértices a dibujar
      \param type: tipo de los índices (únicamente se permiten GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT y GL_UNSIGNED_INT)
      \param offset: posición (en bytes desde el comienzo del buffer vinculado
      a GL_ELEMENT_ARRAY_BUFFER) del primer índice a dibujar
      \param primcount número de instancias a generar
      */
    DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *offset,
      GLsizei primcount) :
      DrawCommand(mode), count(count), type(type), offset(offset), primcount(primcount) {};
    virtual void renderFunc() override {
      glDrawElementsInstanced(mode, count, type, offset, primcount);
    }
  private:
    GLsizei count; GLenum type; const void *offset; GLsizei primcount;
  };


};
#endif
