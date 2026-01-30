#include "crow.h"
#include "Car.h"

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        Car car("Toyota", "Corolla", 2020);
        return "Hello World! I have a " + car.getMake() + " " + car.getModel();
    }); 

    CROW_ROUTE(app, "/healthCheck")([](){
        return "OK";
    });



    app.port(8080).multithreaded().run();
    return 0;
}