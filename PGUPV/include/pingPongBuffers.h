
#pragma once

#include <vector>
#include <mutex>

namespace PGUPV {
    
    /**
     \class PingPongBuffers
     Esta clase gestiona una memoria compartida entre dos threads (un productor y un consumidor).
     El uso es el siguiente:
     
     PingPongBuffers buff(...);
     
     unsigned char *b = buff.lockForWrite();
     // rellenar b
     buff.unlockForWrite();
     
     // en otro thread
     unsigned char *c = buff.lockForRead();
     // leer b
     buff.unlockForRead();
     
     La función unlockForRead descarta el buffer leído. En ningún caso se devolverán los buffers fuera
     de secuencia (es decir, devolver el buffer n, y luego el n-1).
     Hay varias políticas que deciden cómo se inserta un nuevo buffer y cómo se lee un buffer, descritas más adelante.

     */
    
    class PingPongBuffers {
    public:
        /**
         Política de manejo de los buffers:
         
         OnlyNewest: siempre se devuelve el buffer más reciente (en el caso de que hubiera más de un buffer disponible,
         los antiguos se descartan). Siempre se podrá encontrar un buffer para escritura
         DiscardOldest: PingPongBuffers::lockForWrite siempre funciona, reemplazando el buffer más antiguo,
         aunque no se haya procesado. En el caso de que haya más de un buffer almacenado, se devuelve el más antiguo
         NoDiscard: si no hay sitio, PingPongBuffers::lockForWrite devuelve nullptr. En el caso de que haya más 
         de un buffer almacenado, se devuelve el más antiguo
         */
        enum class Policy { OnlyNewest, DiscardOldest, NoDiscard };
        PingPongBuffers(unsigned int width, unsigned int height, unsigned int bpp,
                        Policy policy = Policy::OnlyNewest);
        unsigned char *lockForRead();
        void unlockForRead();
        unsigned char *lockForWrite();
        void unlockForWrite();
        bool isEmpty() const;
    private:
        static const int NUM_BUFFERS = 2;
        Policy policy;
        std::vector<unsigned char> buffers[NUM_BUFFERS];
        long ids[NUM_BUFFERS];
        std::mutex m;
        int nextId = 1;
        inline int findMaxId() {
            auto maxIdx = 0;
            for (int i = 1; i< NUM_BUFFERS; i++)
                if (ids[i] > ids[maxIdx])
                    maxIdx = i;
            return maxIdx;
        };
        inline int findMinId() {
            auto minIdx = 0;
            for (int i = 1; i < NUM_BUFFERS; i++)
                if (ids[i] < ids[minIdx])
                    minIdx = i;
            return minIdx;
        }
    };
};
