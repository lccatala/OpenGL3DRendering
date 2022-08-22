#pragma once // 2016


#include "GL/glew.h"

namespace PGUPV {
    /**
\class Query
     
     Clase para hacer consultas a GL.
     Ejemplo de uso:

     Query q(GL_PRIMITIVES_GENERATED);

     render() {
       ...
       q.begin();
       ...dibujar escena...
       q.end();
     
       std::cout << q.getResult();
     }

     Los objetos query se pueden reutilizar varias veces. Cada vez que se llama a
     begin() su valor se resetea a cero.
     Ver: http://docs.gl/gl4/glCreateQueries, http://docs.gl/gl4/glBeginQuery, http://docs.gl/gl4/glEndQuery
     
     */
    class Query {
    public:
        /**
         \param type especifica el tipo de consulta a realizar (GL_SAMPLES_PASSED,
           GL_ANY_SAMPLES_PASSED, GL_TIME_ELAPSED, GL_PRIMITIVES_GENERATED...)
         */
        Query(GLenum type);
        ~Query();
        /** 
         Inicia la recogida de información para la consulta
         */
        void begin();
        /**
         Inicia la recogida de información para la consulta en el stream indicado
         \param stream número de stream a consultar (0..3)
         */
        void beginIndexed(unsigned int stream);
        /**
         Termina la recogida de información para la consulta
         */
        void end();
        
        /**
         return true si el resultado está disponible
         */
        bool isResultAvailable();
        /**
         Esta llamada puede bloquear hasta que OpenGL complete el trabajo necesario para calcular
         el resultado. Puedes usar la llamada Query#isResultAvailable para saber si el resultado
         está disponible.
         \return el resultado como un entero sin signo de 32 bits
         */
        GLuint getResult();
        /**
         Esta llamada puede bloquear hasta que OpenGL complete el trabajo necesario para calcular
         el resultado. Puedes usar la llamada Query#isResultAvailable para saber si el resultado
         está disponible.
         \return el resultado como un entero sin signo de 64 bits
         */
        GLint64 getResult64();
        /**
         \return la hora actual para OpenGL. No necesita una Query
         \warning ¡Cuidado! Mac OS X siempre devuelve 0
         */
        static GLint64 getGLTimeStamp();
         
    private:
        GLenum type;
        GLint stream;
        GLuint id;
    };
};
