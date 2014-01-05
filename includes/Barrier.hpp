#ifndef BARRIER__HPP
#define BARRIER__HPP

#include <semaphore.h>
#include <vector>

namespace Anakin {

class Barrier {
    public:
        Barrier(int threads);
        void request(int threadID);
        void leave(int threadID);
    protected:
    private:
        bool _request(int threadID);
        bool arrayOr(std::vector<bool>* a, std::vector<bool>* b);
        std::vector<bool>* barrierStatus;
        std::vector<bool>* allowedComponents;
        std::vector<bool>* finalizedComponents;
        sem_t* reqNLeaveMut;
        sem_t* barrier;
};

};

#endif // BARRIER__HPP
