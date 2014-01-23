#include "Barrier.hpp"
#include <iostream>
#include <vector>

using namespace Anakin;
using namespace std;



Barrier::Barrier(int threads) {
    if (sem_init (&this->reqNLeaveMut, 0, 1) != 0) {
        cout << "Barrier#Barrier: error initializing semaphore\n";
        exit(-1);
    }
    if (sem_init (&this->forSem, 0, 1) != 0) {
        cout << "Barrier#Barrier: error initializing semaphore\n";
        exit(-1);
    }
    this->barrier = new vector<sem_t>(threads);
    for (int b = 0; b < threads; b++) {
        if (sem_init (&this->barrier->at(b), 0, 0) != 0) {
            cout << "Barrier#request: error initializing semaphore\n";
            exit(-1);
        }
    }
    this->threads = threads;
    this->barrierStatus = new vector<bool>(threads, false);
    this->allowedComponents = new vector<bool>(threads, false);
    this->finalizedComponents = new vector<bool>(threads, false);
}

void Barrier::request(int threadID) {
    cout << "request from: " << threadID << " waiting for others" << endl;
    while (!_request(threadID)) {
        sem_wait(&this->barrier->at(threadID));
    }
    cout << threadID << " crossed the barrier" << endl;
    sem_wait(&this->forSem);
    for (int b = 0; b < this->threads; b++) {
        if (b != threadID) {
            sem_post(&this->barrier->at(b));
        }
    }
    sem_post(&this->forSem);
}

bool Barrier::_request(int threadID) {
    sem_wait(&this->reqNLeaveMut);
    if (this->allowedComponents->at(threadID)) {
        this->allowedComponents->at(threadID) = false;
        sem_post(&this->reqNLeaveMut);
        return true;
    }
    this->barrierStatus->at(threadID) = true;
    bool pass = arrayOr(this->barrierStatus, this->finalizedComponents);
    if (pass) {
        fill(this->barrierStatus->begin(), this->barrierStatus->end(), false);
        fill(this->allowedComponents->begin(), this->allowedComponents->end(), true);
        this->allowedComponents->at(threadID) = false;
        sem_post(&this->reqNLeaveMut);
        return true;
    }
    sem_post(&this->reqNLeaveMut);
    return false;
}

void Barrier::leave(int threadID) {
    sem_wait(&this->reqNLeaveMut);
    this->finalizedComponents->at(threadID) = true;
    for (int b = 0; b < this->threads; b++) {
        if (b != threadID) {
            sem_post(&this->barrier->at(b));
        }
    }
    sem_post(&this->reqNLeaveMut);
}

bool Barrier::arrayOr(vector<bool>* a, vector<bool>* b) {
    bool res = true;
    for (uint i = 0; i < a->size() && res; i++) {
        res = a->at(i) || b->at(i);
    }
    return res;
}
