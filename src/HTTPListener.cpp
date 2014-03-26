#include "HTTPListener.hpp"
#include <iostream>

using namespace Anakin;


HTTPListener* HTTPListener::getInstance(  std::string port,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                            int threads) {

    if (HTTPListener::instance == NULL || port != HTTPListener::port) {
        if (HTTPListener::instance != NULL) {
            mg_destroy_server(&(HTTPListener::server));
            delete HTTPListener::instance;
        }
        HTTPListener::instance = new HTTPListener(port, readingQueue, writtingQueue, threads);
    }
    return HTTPListener::instance;
}

void HTTPListener::start() {
    HTTPListener::running = true;
    std::cout << "Starting on port : " << mg_get_option(HTTPListener::server, "listening_port") << std::endl;
    while (HTTPListener::running) {
        if (HTTPListener::running) mg_poll_server(HTTPListener::server,2000);
    }
    mg_destroy_server(&(HTTPListener::server));
}

//PROTECTED

HTTPListener::HTTPListener( std::string port,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                            int threads) {
    HTTPListener::server = mg_create_server(NULL, ev_handler);
    mg_set_option(HTTPListener::server, "listening_port", port.c_str());
    std::string sthreads = std::to_string(threads);
    mg_set_option(HTTPListener::server, "num_threads", sthreads.c_str());
    HTTPListener::readingQueue = readingQueue;
    HTTPListener::writtingQueue = writtingQueue;
    HTTPListener::running = false;
}

int HTTPListener::ev_handler(struct mg_connection *conn, enum mg_event ev) {
    int result = MG_FALSE;

    if (ev == MG_REQUEST) {
        std::string method = conn->request_method;
        std::string action = conn->uri;
        std::string body(conn->content, conn->content_len);
        char request = method=="POST"?(method=="GET"?HTTPSocket::GET:HTTPSocket::RESPONSE):HTTPSocket::POST;
        int status = conn->status_code;
        HTTPSocket::MessageData* md = new HTTPSocket::MessageData(action, body, request, status);
        if (strcmp(body.c_str(), "stop") == 0) {
            HTTPListener::running = false;
        }
        HTTPListener::readingQueue->push(md);
        HTTPSocket::MessageData* rd;
        if (!HTTPListener::running) {
            //std::cout << "sending stop received" << std::endl;
            std::string r_action = "";
            std::string r_body = "stop received";
            rd = new HTTPSocket::MessageData(r_action.c_str(), r_body.c_str(), HTTPSocket::RESPONSE, 200);
            HTTPListener::writtingQueue->push(rd);
        }
        HTTPListener::writtingQueue->pop(rd);
        mg_send_status(conn, rd->status);
        std::string cc_header = "connection";
        std::string cc_value = "close header";
        mg_send_header(conn, cc_header.c_str(), cc_value.c_str());
        std::string cl_header = "Content-Lenght";
        std::string cl_value = std::to_string(rd->body.size());
        mg_send_header(conn, cl_header.c_str(), cl_value.c_str());
        mg_send_data(conn, rd->body.c_str(), rd->body.size());
        result = MG_CLOSE;
    } else if (ev == MG_AUTH) {
        result = MG_TRUE;
    }

    return result;
}

HTTPListener* HTTPListener::instance = NULL;
std::string HTTPListener::port = "";
bool HTTPListener::running = false;
struct mg_server * HTTPListener::server;
tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* HTTPListener::readingQueue;
tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* HTTPListener::writtingQueue;
