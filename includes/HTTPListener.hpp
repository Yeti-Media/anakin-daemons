#ifndef HTTPLISTENER_HPP
#define HTTPLISTENER_HPP

#include "mongoose.h"
#include <tbb/concurrent_queue.h>
#include <vector>
#include "HTTPSocket.hpp"

namespace Anakin {

class HTTPListener {
    public:
        static HTTPListener* getInstance(   std::string port,
                                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                                            int threads);
        void start();
    protected:
        static HTTPListener* instance;
        HTTPListener(   std::string port,
                        tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                        tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                        int threads);
        static std::string port;
        static bool running;
        static int ev_handler(struct mg_connection *conn, enum mg_event ev);
        static struct mg_server *server;
        static tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue;
        static tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue;
    private:

};
};

#endif // HTTPLISTENER_HPP
