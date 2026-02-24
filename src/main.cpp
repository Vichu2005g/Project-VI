#include "crow.h"
#include "Car.h"
#include "database.h"
#include "CarRoutes.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Function tjat allows to read file contents
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    // Start App
    crow::SimpleApp app;
    
    // Start database
    Database db("data/cars.db");
    
    if (!db.initialize()) {
        std::cerr << "Failed to start Database" << std::endl;
        return 1;
    }
    
    std::cout << "Database up and running" << std::endl;
    
    // setting up routes
    CarRoutes::setupRoutes(app, db);
    std::cout << "API routes configured!" << std::endl;
    
CROW_ROUTE(app, "/")([](){
    std::string html = readFile("frontend/index.html");
    if (html.empty()) return crow::response(404, "Frontend not found");
    auto res = crow::response(html);
    res.add_header("Content-Type", "text/html");
    return res;
});

CROW_ROUTE(app, "/app.js")([](){
    std::string js = readFile("frontend/app.js");
    if (js.empty()) return crow::response(404, "JavaScript not found");
    auto res = crow::response(js);
    res.add_header("Content-Type", "application/javascript");
    return res;
});

CROW_ROUTE(app, "/style.css")([](){
    std::string css = readFile("frontend/style.css");
    if (css.empty()) return crow::response(404, "CSS not found");
    auto res = crow::response(css);
    res.add_header("Content-Type", "text/css");
    return res;
});

CROW_ROUTE(app, "/add-car.html")([](){
    std::string html = readFile("frontend/add-car.html");
    if (html.empty()) return crow::response(404, "Page not found");
    auto res = crow::response(html);
    res.add_header("Content-Type", "text/html");
    return res;
});

CROW_ROUTE(app, "/stats.html")([](){
    std::string html = readFile("frontend/stats.html");
    if (html.empty()) return crow::response(404, "Page not found");
    auto res = crow::response(html);
    res.add_header("Content-Type", "text/html");
    return res;
});

CROW_ROUTE(app, "/home.js")([](){
    std::string js = readFile("frontend/home.js");
    if (js.empty()) return crow::response(404, "JavaScript not found");
    auto res = crow::response(js);
    res.add_header("Content-Type", "application/javascript");
    return res;
});

CROW_ROUTE(app, "/add-car.js")([](){
    std::string js = readFile("frontend/add-car.js");
    if (js.empty()) return crow::response(404, "JavaScript not found");
    auto res = crow::response(js);
    res.add_header("Content-Type", "application/javascript");
    return res;
});

CROW_ROUTE(app, "/stats.js")([](){
    std::string js = readFile("frontend/stats.js");
    if (js.empty()) return crow::response(404, "JavaScript not found");
    auto res = crow::response(js);
    res.add_header("Content-Type", "application/javascript");
    return res;
});

    
    // Starting the server
    std::cout << "\n===================" << std::endl;
    std::cout << "Car Inventory " << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "Server running on http://localhost:8080" << std::endl;
    std::cout << "=======================\n" << std::endl;
    
    app.port(8080).multithreaded().run();
    
    return 0;
}