#include "HTTPListener.hpp"
#include <iostream>
#include<ctime>
#include "Constants.hpp"

using namespace Anakin;


HTTPListener* HTTPListener::getInstance(  std::string port,
                            tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                            BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
                            int threads) {

    if (HTTPListener::instance == NULL || port != HTTPListener::port) {
        if (HTTPListener::instance != NULL) {
            mg_destroy_server(&(HTTPListener::server));
            delete HTTPListener::instance;
        }
        HTTPListener::instance = new HTTPListener(port, readingQueue, writtingQueue, threads);
    }
    srand(time(0));
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
                            BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
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
        std::string request;
        int reqID;
        if (strcmp(body.c_str(), "stop") == 0) {
            HTTPListener::running = false;
            request = body;
            reqID = 0;
        } else {
            request = rw.jsonReqToString(rw.parseJSON(body.c_str()));
            reqID = generateRandomID();
            std::string sreqID = std::to_string(reqID);
            request += " -" + Constants::PARAM_REQID + " " + sreqID;
        }
        char requestType = method=="POST"?(method=="GET"?HTTPSocket::GET:HTTPSocket::RESPONSE):HTTPSocket::POST;
        int status = conn->status_code;
        HTTPSocket::MessageData* md = new HTTPSocket::MessageData(action, request, requestType, status);

        HTTPListener::readingQueue->push(md);
        HTTPSocket::MessageData* rd;
        if (!HTTPListener::running) {
            std::string r_action = "";
            std::string r_body = "stop received";
            rd = new HTTPSocket::MessageData(r_action.c_str(), r_body.c_str(), HTTPSocket::RESPONSE, 200);
            HTTPListener::writtingQueue->put(reqID, rd);
        }
        rd = HTTPListener::writtingQueue->get(reqID);
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
BlockingMap<int, HTTPSocket::MessageData*>* HTTPListener::writtingQueue;
ResultWriter HTTPListener::rw;

//PRIVATE

int HTTPListener::generateRandomID() {
    int r1 = rand() % 10;
    int r2 = rand() % 10;
    int r3 = rand() % 10;
    int r4 = rand() % 10;
    int r5 = rand() % 10;
    int r6 = rand() % 10;
    int randomValue = r1 + (r2*10) + (r3*100) + (r4*1000) + (r5*10000) + (r6*100000);
    std::cout << "randomValue: " << randomValue << std::endl;
    return randomValue;
}
