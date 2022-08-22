#ifndef _STOCK_MODELS_H
#define _STOCK_MODELS_H 2011

/** \file stockModels.h

\author Paco Abad

*/

#include "model.h"
#include "group.h"
#include "uboMaterial.h"

namespace PGUPV {

  // Color por defecto para todos los modelos a los que se les puede asignar un
  // color
  static const glm::vec4 DEFAULT_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  /**
  \class Axes
  Ejes coordenados, con los siguientes colores: X -> rojo, Y -> verde, Z -> azul
  */
  class Axes : public Model {
  public:
    /**
    Constructor
    \param length longitud de los ejes
    */
    explicit Axes(float length = 1.0f);
  };

  /**
  \class WireBox
  Caja dibujada en alámbrico (sólo se dibujan sus aristas)
  */
  class WireBox : public Model {
  public:
    /**
    Constructor
    \param size Longitud del lado del cubo
    \param color Color del cubo
    */
    explicit WireBox(float size = 1.0f, const glm::vec4 &color = DEFAULT_COLOR);
    /**
    Constructor
    \param xsize Ancho del cubo
    \param ysize Alto del cubo
    \param zsize Profundidad del cubo
    \param color Color
    */
    WireBox(float xsize, float ysize, float zsize,
      const glm::vec4 &color = DEFAULT_COLOR);
    /**
    Constructor
    \param bbox Caja de inclusión 
    \param color color de la caja (opcional)
    */
    WireBox(const BoundingBox &bbox, const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class Rect
   Cuadrilátero definido en el plano XY centrado en el origen con el tamaño
   indicado en constructor
   (1x1 por defecto).
   Incluye vértices, normales, colores y texturas.
   La normal apunta a +Z
   */
  class Rect : public Model {
  public:
    /**
     Construye un rectángulo con los parámetros indicados.
     \param width Ancho
     \param height Alto
     \param color Color
     \param nVertX Número de vértices en horizontal
     \param nVertY Número de vértices en vertical
     */
    Rect(float width = 1.0f, float height = 1.0f, const glm::vec4 &color = DEFAULT_COLOR,
      uint nVertX = 2, uint nVertY = 2);
    /**
  \return Devuelve la normal del modelo (cuidado! en el sistema de coordenadas
  local)
  */
    glm::vec3 normal() { return glm::vec3(0.0f, 0.0f, 1.0f); };
    /**
  Devuelve la ecuación del plano (cuidado! en el sistema de coordenadas local).
  Si quieres transformarlo según una matriz de transformación M, tienes que
  hacer:
  Pp = glm::transpose(glm::inverse(M))*P;
  \return Los coeficientes de la ecuación del plano (Ax+By+Cz+D = 0)
  */
    glm::vec4 getPlaneEquation() { return glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); }
  };

  /**
   \class Disk
   Disco definido en el plano XY centrado en el origen. El radio interior define
   el
   tamaño del agujero (si es cero, entonces el objeto es un círculo)
   */
  class Disk : public Model {
  public:
    /**
     Construye un disco con los parámetros indicados
     \param r_in Radio interior
     \param r_out Radio exterior
     \param slices Sectores que componen el disco
     \param rings Anillos a lo largo del anillo
     \param color Color
     */
    Disk(float r_in = 0.0f, float r_out = 1.0f, uint slices = 20, uint rings = 4,
      const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
  \class Box
  Caja centrada en el origen
  Define posiciones, normales, coordenadas de textura y un color.
  Compuesta por 6 caras, con 4 vértices por cara
  */

  class Box : public Model {
  public:
    /**
      Define un cubo con un tamaño y color dados (por defecto un cubo de 1x1x1 y negro)
      \param size longitud del lado del cubo
      \param color color del cubo

    */
    explicit Box(float size = 1.0, const glm::vec4 &color = DEFAULT_COLOR);
    /**
      Define una caja del tamaño y color dados
      \param xsize longitud del lado en el eje X
      \param ysize longitud del lado en el eje y
      \param zsize longitud del lado en el eje z
      \param color color de la caja
    */
    Box(float xsize, float ysize, float zsize, const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
  \class Pyramid
  Pirámide apoyada en el origen
  Define posiciones, normales y un color.
  Compuesta por 5 caras
  */

  class Pyramid : public Model {
  public:
	  /**
		Define una pirámide con un tamaño y color dados (por defecto la base tiene lado 1 y altura 1 y negro)
		\param size longitud del lado de la base
		\param height altura de la pirámide
		\param base si true, incluir la base de la pirámide
		\param color color de la pirámide

	  */
	  Pyramid(float size = 1.0f, float height = 1.0f, bool base = true, const glm::vec4 &color = DEFAULT_COLOR);
  };


  /**
   \class Sphere
   Esfera centrada en el origen.
   Define posiciones, normales, coordenadas de textura y un color
   Se puede especificar el número de rodajas a lo largo del eje Y (stacks)
   y el número de sectores en el plano XZ
   */
  class Sphere : public Model {
  public:
    /**
     Construye una esfera con los parámetros especificados
     \param radius Radio de la esfera
     \param stacks Número de rodajas en vertical
     \param slices Número de rodajas radiales
     \param color Color
     */
    Sphere(float radius = 1.0f, uint stacks = 10, uint slices = 20,
      const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
  \class Sphere2

  Esfera centrada en el origen.
  Define posiciones, normales, tangentes, coordenadas de textura y un
  color
  El nivel de subdivisión define el número único de vértices.
  Implementada según:
  http://www.iquilezles.org/www/articles/patchedsphere/patchedsphere.htm
  */
  class Sphere2 : public Model {
  public:
    /**
    Constructor que define las características de la esfera teselada
    \param radius Radio de la esfera
    \param subdivision Define el nivel de detalle de la esfera. El número de
    vértices únicos
    de la esfera está definido por la siguiente ecuación:
    numero_unico_vertices = 6 * N^2 + 12N + 8, donde N es el nivel de subdivision
    (para N=0, hay 8 vértices, para N=1 26, para N=2 56, 98, 152, 218...)
    \param color Color de la esfera
    */
    Sphere2(float radius = 1.0f, uint subdivision = 6,
      const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class Cylinder

   Cilindro centrado en el origen y paralelo al eje Y. NO define las 'tapas'.
   Define posiciones, normales, coordenadas de textura y un color
   Se puede especificar el número de rodajas a lo largo del eje Y (stacks)
   y el número de sectores en el plano XZ

   \warning Para definir un cono, usa un radio muy pequeño (en vez de 0.0), para
   que no se generen polígonos degenerados
   */

  class Cylinder : public Model {
  public:
    Cylinder(float r_bottom = 1.0f / 6.2832f, float r_top = 1.0f / 6.2832f,
      float height = 1.0f, uint stacks = 10, uint slices = 20,
      const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class TeapotPatches

   Tetera formada por parches bicúbicos de Bezier. ¡Cuidado! Sólo funciona al usar
   shaders de teselación.
   La tetera está apoyada y centrada sobre el plano Z = 0, con la tapa en la
   dirección +Y.
   */
  class TeapotPatches : public Model {
  public:
    /**
     Tetera formada por varios parches de Bezier. Cada parche está definido por 16
     vértices.
     */
    TeapotPatches();
  };

  // Flecha que parte del origen en la dirección positiva de las Z
  class Arrow : public Group {
  public:
    static std::shared_ptr<Arrow> build(float length, float shaft_radio, float head_radio, 
      float head_ratio, const glm::vec4 &head_color, const glm::vec4 &shaft_color);
  protected:
    Arrow() {};
  };

  class BoneWidget : public Group {
  public:
	  static std::shared_ptr<BoneWidget> build();
  protected:
	  BoneWidget() {};
  };

  /**
  \class ScreenPolygon

  Al dibujar una instancia de esta clase, se dibujará un polígono que ocupará toda
  la pantalla (no hace
  falta establecer las matrices de proyección, esta clase se encarga de todo).
  Define posiciones, color y coordenadas de textura (TEX_COORD0: entre 0, 0 y 1,1)
  */
  class ScreenPolygon : public Model {
  public:
    explicit ScreenPolygon(const glm::vec4 &color = DEFAULT_COLOR);
    void render() override;
  };
};

#endif
