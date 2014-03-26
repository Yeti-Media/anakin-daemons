#ifndef HTTPSOCKET_HPP
#define HTTPSOCKET_HPP

#include "mongoose.h"
#include <tbb/concurrent_queue.h>
#include "semaphore.h"
#include <vector>
#include <pthread.h>

namespace Anakin {

class HTTPSocket {
    public:
        HTTPSocket(std::string port, int threads);

        void respond(std::string body, bool statusOK);

        std::string read();

        static const char POST = 0;
        static const char GET = 1;
        static const char RESPONSE = 2;

        void setShowComs(bool v) {
            showComs = v;
        }

        struct MessageData {
            std::string method;
            std::string action;
            std::string body;
            char request;
            int status;
            MessageData (   std::string action,
                            std::string body,
                            char request,
                            int status
                        ) : action(action),
                            body(body),
                            request(request),
                            status(status) {
                switch (request) {
                    case HTTPSocket::POST       :   {method = "POST"; break;}
                    case HTTPSocket::GET        :   {method = "GET"; break;}
                    case HTTPSocket::RESPONSE   :   {method = ""; break;}
                }
            }
        };

        struct ListenerArgs {
            std::string port;
            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue;
            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue;
            int threads;
            ListenerArgs(   std::string port,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                            int threads
                        ) : port(port),
                            readingQueue(readingQueue),
                            writtingQueue(writtingQueue),
                            threads(threads) {}
        };

        void stop();

    protected:
    private:
        bool showComs=false;
        struct mg_server *server;
        std::string port;


        sem_t sem;
        pthread_t t;

        tbb::concurrent_bounded_queue<MessageData*>* readingQueue;
        tbb::concurrent_bounded_queue<MessageData*>* writtingQueue;

        void startToListen( std::string port,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                            int threads);
        static void * startListener(void *ptr);
};
};

#endif // HTTPSOCKET_HPP
