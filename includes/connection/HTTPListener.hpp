#ifndef HTTPLISTENER_HPP
#define HTTPLISTENER_HPP

#include "mongoose.h"
#include <tbb/concurrent_queue.h>
#include <vector>
#include "connection/HTTPSocket.hpp"
#include "output/ResultWriter.hpp"
#include "utils/BlockingMap.hpp"

namespace Anakin {

class HTTPListener {
    public:
        static HTTPListener* getInstance(   std::string port,
                                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                                            BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
                                            int threads);
        void start();
    protected:
        static HTTPListener* instance;
        HTTPListener(   std::string port,
                        tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                        BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
                        int threads);
        static std::string port;
        static bool running;
        static int ev_handler(struct mg_connection *conn, enum mg_event ev);
        static struct mg_server *server;
        static tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue;
        static BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue;
        static ResultWriter rw;
    private:
        static int generateRandomID();

};

};

#endif // HTTPLISTENER_HPP
