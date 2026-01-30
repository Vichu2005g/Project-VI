#include "crow.h"
#include "Car.h"
#include "CarRepository.h"
#include <iostream>

int main() {
    crow::SimpleApp app;

    // Initialize database
    CarRepository carRepo("cars.db");
    if (!carRepo.initDatabase()) {
        std::cerr << "Failed to initialize database!" << std::endl;
        return 1;
    }
    std::cout << "Database initialized successfully!" << std::endl;

    CROW_ROUTE(app, "/")([](){
        return "Car Management API - Database Ready!";
    }); 

    CROW_ROUTE(app, "/healthCheck")([](){
        return "OK";
    });

    // Test endpoint to add a car and retrieve it
    CROW_ROUTE(app, "/test")([&carRepo](){
        // Create a test car
        Car testCar("Toyota", "Corolla", 2020);
        testCar.setVinId(12345);
        testCar.setMileage(50000);
        
        if (carRepo.createCar(testCar)) {
            // Retrieve all cars
            auto cars = carRepo.getAllCars();
            std::string result = "Database test successful! Cars in database: " + std::to_string(cars.size());
            return result;
        }
        
        return std::string("Failed to add car to database");
    });

    app.port(8080).multithreaded().run();
    return 0;
}