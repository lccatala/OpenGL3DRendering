#include <limits.h>
#include "pingPongBuffers.h"
#include "log.h"

using PGUPV::PingPongBuffers;

PingPongBuffers::PingPongBuffers(unsigned int width, unsigned int height, unsigned int bpp, Policy policy)
: policy(policy) {
    for (unsigned int i = 0; i < NUM_BUFFERS; i++) {
        buffers[i].resize(width * height * bpp / 8);
        ids[i] = 0;
    }
}

unsigned char *PingPongBuffers::lockForRead() {
    std::lock_guard<std::mutex> lock{m};
    int maxIdx = findMaxId();
    if (ids[maxIdx] == 0)
        return nullptr;
    if (ids[maxIdx] == LONG_MAX)
        ERRT("Ya estaba bloqueado para lectura");

    if (policy != Policy::OnlyNewest) {
        int minNonZero = maxIdx;
        for (int i = (maxIdx + 1) % NUM_BUFFERS; i != maxIdx; i = ( i + 1) % NUM_BUFFERS)
            if (ids[i] > 0 && ids[i] < LONG_MAX && ids[i] < ids[minNonZero])
                minNonZero = i;
        maxIdx = minNonZero;
    }
    ids[maxIdx] = LONG_MAX;
    return &buffers[maxIdx][0];
}

void PingPongBuffers::unlockForRead() {
    std::lock_guard<std::mutex> lock{m};
    int maxIdx = findMaxId();
    if (ids[maxIdx] != LONG_MAX)
        ERRT("No se ha bloqueado antes para lectura");
    ids[maxIdx] = 0;
}

unsigned char * PingPongBuffers::lockForWrite() {
    std::lock_guard<std::mutex> lock{m};
    int minIdx = findMinId();
    if (ids[minIdx] < 0)
        ERRT("Ya estaba bloqueado para escritura");
    if (ids[minIdx] == 0 || policy != Policy::NoDiscard ) {
        ids[minIdx] = LONG_MIN;
        return &buffers[minIdx][0];
        
    }
    return nullptr;
}

void PingPongBuffers::unlockForWrite() {
    std::lock_guard<std::mutex> lock{m};
    int minIdx = findMinId();
    if (ids[minIdx] != LONG_MIN)
        ERRT("No se ha bloqueado antes para escritura");
    ids[minIdx] = nextId++;
    if (policy == Policy::OnlyNewest) {
        // Discard others
        for (int i = (minIdx + 1) % NUM_BUFFERS; i != minIdx; i = (i + 1) % NUM_BUFFERS)
            if (ids[i] > 0 && ids[i] < LONG_MAX)
                ids[i] = 0;
    }
}

bool PingPongBuffers::isEmpty() const {
    for (auto id : ids)
        if (id != 0)
            return false;
    return true;
}
