#include "crow.h"

int main() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        return "Hello World!";
    }); 

    CROW_ROUTE(app, "/healthCheck")([](){
        return "OK";
    });



    app.port(8080).multithreaded().run();
    return 0;
}