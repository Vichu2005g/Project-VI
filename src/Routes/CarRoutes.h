#pragma once
#include "crow.h"
#include "database.h"
#include "Car.h"
#include <vector>

class CarRoutes {
public:
    static void setupRoutes(crow::SimpleApp& app, Database& db) {
        
        // GET /api/cars which gets all cars
        CROW_ROUTE(app, "/api/cars")
        .methods("GET"_method)
        ([&db](){
            std::vector<Car> cars = db.getAllCars();
            
            crow::json::wvalue response;
            response = crow::json::wvalue::list();
            
            for (size_t i = 0; i < cars.size(); i++) {
                response[i]["id"] = cars[i].getCarId();
                response[i]["make"] = cars[i].getMake();
                response[i]["model"] = cars[i].getModel();
                response[i]["year"] = cars[i].getYear();
                response[i]["price"] = cars[i].getPrice();
                response[i]["mileageKm"] = cars[i].getMileage();
                response[i]["color"] = cars[i].getColor();
                response[i]["vin"] = cars[i].getVin();
                response[i]["createdAt"] = cars[i].getCreatedAt();
                response[i]["updatedAt"] = cars[i].getUpdatedAt();
            }
            
            return crow::response(200, response);
        });
        
        // GET /api/cars/:id  gets car by ID
        CROW_ROUTE(app, "/api/cars/<int>")
        .methods("GET"_method)
        ([&db](int id){
            bool found = false;
            Car car = db.getCarById(id, found);
            
            if (!found) {
                crow::json::wvalue error;
                error["error"] = "Car not found";
                return crow::response(404, error);
            }
            
            crow::json::wvalue response;
            response["id"] = car.getCarId();
            response["make"] = car.getMake();
            response["model"] = car.getModel();
            response["year"] = car.getYear();
            response["price"] = car.getPrice();
            response["mileageKm"] = car.getMileage();
            response["color"] = car.getColor();
            response["vin"] = car.getVin();
            response["createdAt"] = car.getCreatedAt();
            response["updatedAt"] = car.getUpdatedAt();
            
            return crow::response(200, response);
        });
        
        // POST /api/cars - Create new car
        CROW_ROUTE(app, "/api/cars")
        .methods("POST"_method)
        ([&db](const crow::request& req){
            auto body = crow::json::load(req.body);
            
            if (!body) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            // Validation
            if (!body.has("make") || !body.has("model") || !body.has("year")) {
                crow::json::wvalue error;
                error["error"] = "Missing required fields: make, model, year";
                return crow::response(400, error);
            }
            
            Car car;
            car.setMake(body["make"].s());
            car.setModel(body["model"].s());
            car.setYear(body["year"].i());
            
            if (body.has("price")) car.setPrice(body["price"].d());
            if (body.has("mileageKm")) car.setMileage(body["mileageKm"].i());
            if (body.has("color")) car.setColor(body["color"].s());
            if (body.has("vin")) car.setVin(body["vin"].s());
            
            int newId = 0;
            if (!db.insertCar(car, newId)) {
                crow::json::wvalue error;
                error["error"] = "Failed to create car";
                return crow::response(500, error);
            }
            
            bool found = false;
            Car createdCar = db.getCarById(newId, found);
            
            crow::json::wvalue response;
            response["id"] = createdCar.getCarId();
            response["make"] = createdCar.getMake();
            response["model"] = createdCar.getModel();
            response["year"] = createdCar.getYear();
            response["price"] = createdCar.getPrice();
            response["mileageKm"] = createdCar.getMileage();
            response["color"] = createdCar.getColor();
            response["vin"] = createdCar.getVin();
            
            auto res = crow::response(201, response);
            res.add_header("Location", "/api/cars/" + std::to_string(newId));
            return res;
        });
        
        // PUT /api/cars/:id - Update car 
        CROW_ROUTE(app, "/api/cars/<int>")
        .methods("PUT"_method)
        ([&db](const crow::request& req, int id){
            if (!db.carExists(id)) {
                crow::json::wvalue error;
                error["error"] = "Car not found";
                return crow::response(404, error);
            }
            
            auto body = crow::json::load(req.body);
            
            if (!body) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            Car car;
            car.setCarId(id);
            car.setMake(body["make"].s());
            car.setModel(body["model"].s());
            car.setYear(body["year"].i());
            
            if (body.has("price")) car.setPrice(body["price"].d());
            if (body.has("mileageKm")) car.setMileage(body["mileageKm"].i());
            if (body.has("color")) car.setColor(body["color"].s());
            if (body.has("vin")) car.setVin(body["vin"].s());
            
            if (!db.updateCar(id, car)) {
                crow::json::wvalue error;
                error["error"] = "Failed to update car";
                return crow::response(500, error);
            }
            
            bool found = false;
            Car updatedCar = db.getCarById(id, found);
            
            crow::json::wvalue response;
            response["id"] = updatedCar.getCarId();
            response["make"] = updatedCar.getMake();
            response["model"] = updatedCar.getModel();
            response["year"] = updatedCar.getYear();
            response["price"] = updatedCar.getPrice();
            response["mileageKm"] = updatedCar.getMileage();
            
            return crow::response(200, response);
        });
        
        // DELETE /api/cars/:id - Delete car
        CROW_ROUTE(app, "/api/cars/<int>")
        .methods("DELETE"_method)
        ([&db](int id){
            if (!db.carExists(id)) {
                crow::json::wvalue error;
                error["error"] = "Car not found";
                return crow::response(404, error);
            }
            
            if (!db.deleteCar(id)) {
                crow::json::wvalue error;
                error["error"] = "Failed to delete car";
                return crow::response(500, error);
            }
            
            return crow::response(204);
        });
        
        // OPTIONS /api/cars 
        CROW_ROUTE(app, "/api/cars")
        .methods("OPTIONS"_method)
        ([](){
            auto res = crow::response(204);
            res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            return res;
        });
        
        // OPTIONS /api/cars/:id 
        CROW_ROUTE(app, "/api/cars/<int>")
        .methods("OPTIONS"_method)
        ([](int){
            auto res = crow::response(204);
            res.add_header("Access-Control-Allow-Methods", "GET, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            return res;
        });
    }
};