#pragma once
#include "crow.h"
#include "database.h"
#include "Car.h"
#include <vector>
#include <string>
#include "StringUtils.h"
class CarRoutes {
public:
    static void setupRoutes(crow::SimpleApp& app, Database& db) {

        auto getString = [](const crow::json::rvalue& body, const std::string& key) -> std::string {
            if (!body.has(key)) return "";
            if (body[key].t() == crow::json::type::String) return body[key].s();
            return "";
        };

        auto getInt = [](const crow::json::rvalue& body, const std::string& key, int def = 0) -> int {
            if (!body.has(key)) return def;
            if (body[key].t() == crow::json::type::Number) return body[key].i();
            return def;
        };

        auto getDouble = [](const crow::json::rvalue& body, const std::string& key, double def = 0.0) -> double {
            if (!body.has(key)) return def;
            if (body[key].t() == crow::json::type::Number) return body[key].d();
            return def;
        };

        // GET all
        CROW_ROUTE(app, "/api/cars").methods("GET"_method)
        ([&db]() {
            std::vector<Car> cars = db.getAllCars();
            crow::json::wvalue response = crow::json::wvalue::list();

            for (size_t i = 0; i < cars.size(); i++) {
                response[i]["id"] = cars[i].getCarId();
                response[i]["make"] = cars[i].getMake();
                response[i]["model"] = cars[i].getModel();
                response[i]["year"] = cars[i].getYear();
                response[i]["price"] = cars[i].getPrice();
                response[i]["mileageKm"] = cars[i].getMileage();
                response[i]["color"] = cars[i].getColor();
                response[i]["vin"] = cars[i].getVin();
                response[i]["imageDataUrl"] = cars[i].getImageDataUrl(); 
                response[i]["createdAt"] = cars[i].getCreatedAt();
                response[i]["updatedAt"] = cars[i].getUpdatedAt();
            }
            return crow::response(200, response);
        });

        // GET by id
        CROW_ROUTE(app, "/api/cars/<int>").methods("GET"_method)
        ([&db](int id) {
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
            response["imageDataUrl"] = car.getImageDataUrl(); 
            response["createdAt"] = car.getCreatedAt();
            response["updatedAt"] = car.getUpdatedAt();

            return crow::response(200, response);
        });

        // POST create
        CROW_ROUTE(app, "/api/cars").methods("POST"_method)
        ([&db, getString, getInt, getDouble](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }

            if (!body.has("make") || !body.has("model") || !body.has("year") || !body.has("price") || !body.has("mileageKm")) {
                crow::json::wvalue error;
                error["error"] = "Missing required fields: make, model, year, price, mileageKm";
                return crow::response(400, error);
            }

            Car car;
            car.setMake(StringUtils::toTitleCase(getString(body, "make")));          
            car.setModel(StringUtils::toTitleCase(getString(body, "model")));         
            car.setYear(getInt(body, "year"));
            car.setPrice(getDouble(body, "price"));
            car.setMileage(getInt(body, "mileageKm"));

            car.setColor(StringUtils::toTitleCase(getString(body, "color")));         
            car.setVin(StringUtils::toUpperCase(getString(body, "vin")));             
            car.setImageDataUrl(getString(body, "imageDataUrl")); 

            int newId = 0;
            if (!db.insertCar(car, newId)) {
                crow::json::wvalue error;
                error["error"] = "Failed to create car (possible duplicate VIN)";
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
            response["imageDataUrl"] = createdCar.getImageDataUrl(); 

            auto res = crow::response(201, response);
            res.add_header("Location", "/api/cars/" + std::to_string(newId));
            return res;
        });


        // PATCH which is a partial update of the car resource. Only the fields present in the request body will be updated, allowing for more flexible updates without requiring the client to send the entire car object.
CROW_ROUTE(app, "/api/cars/<int>").methods("PATCH"_method)
([&db, getString, getInt, getDouble](const crow::request& req, int id) {
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

    // Get existing car
    bool found = false;
    Car car = db.getCarById(id, found);

    // Only updates the fields that are present in the request body
    if (body.has("make")) car.setMake(getString(body, "make"));
    if (body.has("model")) car.setModel(getString(body, "model"));
    if (body.has("year")) car.setYear(getInt(body, "year"));
    if (body.has("price")) car.setPrice(getDouble(body, "price"));
    if (body.has("mileageKm")) car.setMileage(getInt(body, "mileageKm"));
    if (body.has("color")) car.setColor(getString(body, "color"));
    if (body.has("vin")) car.setVin(getString(body, "vin"));
    if (body.has("imageDataUrl")) car.setImageDataUrl(getString(body, "imageDataUrl"));

    if (!db.updateCar(id, car)) {
        crow::json::wvalue error;
        error["error"] = "Failed to update car";
        return crow::response(500, error);
    }

    Car updatedCar = db.getCarById(id, found);
    crow::json::wvalue response;
    response["id"] = updatedCar.getCarId();
    response["make"] = updatedCar.getMake();
    response["model"] = updatedCar.getModel();
    response["year"] = updatedCar.getYear();
    response["price"] = updatedCar.getPrice();
    response["mileageKm"] = updatedCar.getMileage();
    response["color"] = updatedCar.getColor();
    response["vin"] = updatedCar.getVin();
    response["imageDataUrl"] = updatedCar.getImageDataUrl();

    return crow::response(200, response);
});

// OPTIONS 
CROW_ROUTE(app, "/api/cars").methods("OPTIONS"_method)
([]() {
    auto res = crow::response(204);
    res.add_header("Allow", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return res;
});

CROW_ROUTE(app, "/api/cars/<int>").methods("OPTIONS"_method)
([](int id) {
    auto res = crow::response(204);
    res.add_header("Allow", "GET, PUT, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return res;
});

        // PUT update
       CROW_ROUTE(app, "/api/cars/<int>").methods("PUT"_method)
        ([&db, getString, getInt, getDouble](const crow::request& req, int id) {
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

            if (!body.has("make") || !body.has("model") || !body.has("year") || !body.has("price") || !body.has("mileageKm")) {
                crow::json::wvalue error;
                error["error"] = "Missing required fields: make, model, year, price, mileageKm";
                return crow::response(400, error);
            }

           Car car;
            car.setCarId(id);
            car.setMake(StringUtils::toTitleCase(getString(body, "make")));           
            car.setModel(StringUtils::toTitleCase(getString(body, "model")));           
            car.setYear(getInt(body, "year"));
            car.setPrice(getDouble(body, "price"));
            car.setMileage(getInt(body, "mileageKm"));

            car.setColor(StringUtils::toTitleCase(getString(body, "color")));         
            car.setVin(StringUtils::toUpperCase(getString(body, "vin")));             
            car.setImageDataUrl(getString(body, "imageDataUrl")); 


            if (!db.updateCar(id, car)) {
                crow::json::wvalue error;
                error["error"] = "Failed to update car (possible duplicate VIN)";
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
            response["color"] = updatedCar.getColor();
            response["vin"] = updatedCar.getVin();
            response["imageDataUrl"] = updatedCar.getImageDataUrl(); 

            return crow::response(200, response);
        });

        // DELETE
        CROW_ROUTE(app, "/api/cars/<int>").methods("DELETE"_method)
        ([&db](int id) {
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
    }
};
