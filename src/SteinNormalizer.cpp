#include "SteinNormalizer.hpp"
#include <pthread.h>

using namespace Anakin;
using namespace std;
using namespace cv;

SteinNormalizer::SteinNormalizer(cv::Mat, int segments) {
}

SteinNormalizer::SteinNormalizer() {
    cout << "you are using the testing constructor\n" << endl;
}

void* component(void* arg) {
    pair<int, SteinNormalizer::ComponentArg*>* compArg;
    compArg = (pair<int, SteinNormalizer::ComponentArg*>*)arg;
    cout << "Hello World! Thread ID, " << compArg->first << endl;
    int size = compArg->second->getSize();
    for (int i = 0; i < size; i++) {
        compArg->second->set(compArg->first, compArg->second->get(compArg->first)+1);
        cout << "[";
        for (int e = 0; e < size; e++) {
            cout << compArg->second->get(e);
            if (e+1 < size) {
                cout << ", ";
            }
        }
        cout << "]\n";
    }
    pthread_exit(NULL);
}

Mat SteinNormalizer::normalize(int threadsToRun) {
    SteinNormalizer::ComponentArg* compArg = new SteinNormalizer::ComponentArg(threadsToRun);
    pthread_t threads[threadsToRun];
    int rc;
    for(int i = 0; i < threadsToRun; i++ ){
        cout << "main() : creating  thread, " << i << endl;
        pair<int, SteinNormalizer::ComponentArg*>* arg = new pair<int, SteinNormalizer::ComponentArg*>(i, compArg);
        rc = pthread_create(&threads[i], NULL, component, (void *)arg);
        if (rc){
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }
    cout << "[";
    for (int e = 0; e < compArg->getSize(); e++) {
        cout << compArg->get(e);
        if (e+1 < compArg->getSize()) {
            cout << ", ";
        }
    }
    cout << "]\n";
    pthread_exit(NULL);
    return Mat();
}


vector<Point>* SteinNormalizer::getNeighbors(const Point pos) {
    vector<Point>* neighbors = new vector<Point>(0);
    return neighbors;
}

int SteinNormalizer::getMaxLabel(vector<Point>* neighbors) {
    return 0;
}

vector<int>* SteinNormalizer::getSegmentValues(const int value) {
    vector<int>* segment = new vector<int>(0);
    return segment;
}

int SteinNormalizer::getMean(vector<int>* values) {
    return 0;
}

bool SteinNormalizer::areInSameSegment(const int v1, const int v2) {
    return false;
}
