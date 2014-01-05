#include "Barrier.hpp"
#include <iostream>
#include <vector>

using namespace Anakin;
using namespace std;



Barrier::Barrier(int threads) {
    if (sem_init (reqNLeaveMut, 0, 1) != 0) {
        cout << "Barrier#Barrier: error initializing semaphore\n";
        exit(-1);
    }
    this->barrierStatus = new vector<bool>(threads, false);
    this->allowedComponents = new vector<bool>(threads, false);
    this->finalizedComponents = new vector<bool>(threads, false);
}

void Barrier::request(int threadID) {
    if (sem_init (barrier, 0, 0) != 0) {
        cout << "Barrier#request: error initializing semaphore\n";
        exit(-1);
    }
    while (!_request(threadID)) {
        sem_wait(barrier);
    }
    sem_post(barrier);
}

bool Barrier::_request(int threadID) {
    sem_wait(reqNLeaveMut);
    if (this->allowedComponents->at(threadID)) {
        this->allowedComponents->at(threadID) = false;
        sem_post(reqNLeaveMut);
        return true;
    }
    this->barrierStatus->at(threadID) = true;
    bool pass = arrayOr(this->barrierStatus, this->finalizedComponents);
    if (pass) {
        fill(this->barrierStatus->begin(), this->barrierStatus->end(), false);
        fill(this->allowedComponents->begin(), this->allowedComponents->end(), true);
        this->allowedComponents->at(threadID) = false;
        sem_post(reqNLeaveMut);
        return true;
    }
    sem_post(reqNLeaveMut);
    return false;
}

void Barrier::leave(int threadID) {
    sem_wait(reqNLeaveMut);
    this->finalizedComponents->at(threadID) = true;
    sem_post(barrier);
    sem_post(reqNLeaveMut);
}

bool Barrier::arrayOr(vector<bool>* a, vector<bool>* b) {
    bool res = true;
    for (int i = 0; i < a->size() && res; i++) {
        res = a->at(i) || b->at(i);
    }
    return res;
}
