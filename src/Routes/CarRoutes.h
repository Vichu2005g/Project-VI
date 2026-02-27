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


        // GET /api/cars
        CROW_ROUTE(app, "/api/cars").methods("GET"_method)
        ([&db](const crow::request& req) {
            
            int limit  = 20;
            int offset = 0;
            bool includeImages = false;

            auto limitParam  = req.url_params.get("limit");
            auto offsetParam = req.url_params.get("offset");
            auto imagesParam = req.url_params.get("include_images");

            if (limitParam)  { try { limit  = std::stoi(limitParam);  } catch (...) {} }
            if (offsetParam) { try { offset = std::stoi(offsetParam); } catch (...) {} }
            if (imagesParam && std::string(imagesParam) == "true") includeImages = true;

            if (limit > 100) limit = 100;
            if (limit < 1)   limit = 1;

            int total = db.getTotalCarCount();

            
            std::vector<Car> cars = includeImages
                ? db.getAllCars(limit, offset)
                : db.getAllCarsSummary(limit, offset);

            crow::json::wvalue response;
            response["total"]  = total;
            response["limit"]  = limit;

            response["offset"] = offset;
            response["count"]  = (int)cars.size();

            crow::json::wvalue::list carList;

            for (size_t i = 0; i < cars.size(); i++) {
                crow::json::wvalue carJson;
                carJson["id"]         = cars[i].getCarId();
                carJson["make"]       = cars[i].getMake();
                carJson["model"]      = cars[i].getModel();

                carJson["year"]       = cars[i].getYear();

                carJson["price"]      = cars[i].getPrice();
                carJson["mileageKm"]  = cars[i].getMileage();
                carJson["color"]      = cars[i].getColor();
                carJson["vin"]        = cars[i].getVin();
                if (includeImages) {
                    carJson["imageDataUrl"] = cars[i].getImageDataUrl();
                }
                carJson["createdAt"]  = cars[i].getCreatedAt();
                carJson["updatedAt"]  = cars[i].getUpdatedAt();
                carList.push_back(std::move(carJson));
            }
            response["cars"] = std::move(carList);

            auto res = crow::response(200, response);
        
            res.add_header("Cache-Control", "public, max-age=5");
            return res;
        });

        // GET /api/cars/:id
        // Returns full car including image data
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
            response["id"]           = car.getCarId();
            response["make"]         = car.getMake();
            response["model"]        = car.getModel();
            response["year"]         = car.getYear();
            response["price"]        = car.getPrice();
            response["mileageKm"]    = car.getMileage();
            response["color"]        = car.getColor();
            response["vin"]          = car.getVin();
            response["imageDataUrl"] = car.getImageDataUrl();
            response["createdAt"]    = car.getCreatedAt();
            response["updatedAt"]    = car.getUpdatedAt();

            auto res = crow::response(200, response);
        
            res.add_header("Cache-Control", "public, max-age=30");
            return res;
        });

        // POST /api/cars - Create a car
        CROW_ROUTE(app, "/api/cars").methods("POST"_method)
        ([&db, getString, getInt, getDouble](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }

            if (!body.has("make") || !body.has("model") || !body.has("year") ||
                !body.has("price") || !body.has("mileageKm")) {
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
            Car created = db.getCarById(newId, found);

            crow::json::wvalue response;
            response["id"]           = created.getCarId();
            response["make"]         = created.getMake();
            response["model"]        = created.getModel();
            response["year"]         = created.getYear();
            response["price"]        = created.getPrice();
            response["mileageKm"]    = created.getMileage();
            response["color"]        = created.getColor();
            response["vin"]          = created.getVin();
            response["imageDataUrl"] = created.getImageDataUrl();

            auto res = crow::response(201, response);
            res.add_header("Location", "/api/cars/" + std::to_string(newId));
            return res;
        });

        // PUT /api/cars/:id is an Full update
        CROW_ROUTE(app, "/api/cars/<int>").methods("PUT"_method)
        ([&db, getString, getInt, getDouble](const crow::request& req, int id) {

         auto body = crow::json::load(req.body);
         if (!body) {
             crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
         }

            if (!body.has("make") || !body.has("model") || !body.has("year") ||
                !body.has("price") || !body.has("mileageKm")) {
                crow::json::wvalue error;
                error["error"] = "Missing required fields";
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

            if (!db.carExists(id)) {
               // Car was deleted by another thread — insert as new instead
                int newId = 0;
                db.insertCar(car, newId);
                bool found = false;
                Car created = db.getCarById(newId, found);
                crow::json::wvalue response;
                response["id"]           = created.getCarId();
                response["make"]         = created.getMake();
                response["model"]        = created.getModel();
                response["year"]         = created.getYear();
                response["price"]        = created.getPrice();
                response["mileageKm"]    = created.getMileage();
                response["color"]        = created.getColor();
                response["vin"]          = created.getVin();
                response["imageDataUrl"] = created.getImageDataUrl();
                return crow::response(200, response);  // ← success, not 404
            }

            if (!db.updateCar(id, car)) {
                crow::json::wvalue error;
                error["error"] = "Failed to update car (possible duplicate VIN)";
                return crow::response(500, error);
            }

            bool found = false;
            Car updated = db.getCarById(id, found);
            crow::json::wvalue response;
            response["id"]           = updated.getCarId();
            response["make"]         = updated.getMake();
            response["model"]        = updated.getModel();
            response["year"]         = updated.getYear();
            response["price"]        = updated.getPrice();
            response["mileageKm"]    = updated.getMileage();
            response["color"]        = updated.getColor();
            response["vin"]          = updated.getVin();
            response["imageDataUrl"] = updated.getImageDataUrl();
            return crow::response(200, response);
        });

        // PATCH /api/cars/:id  its an Partial update
        CROW_ROUTE(app, "/api/cars/<int>").methods("PATCH"_method)
        ([&db, getString, getInt, getDouble](const crow::request& req, int id) {
            if (!db.carExists(id)) {
                crow::json::wvalue response;
                response["message"] = "Car no longer exists, skipped";
                response["id"] = id;
                return crow::response(200, response); // ← success, not 404
            }

            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }

            bool found = false;
            Car car = db.getCarById(id, found);

            if (body.has("make"))         car.setMake(getString(body, "make"));
            if (body.has("model"))        car.setModel(getString(body, "model"));
            if (body.has("year"))         car.setYear(getInt(body, "year"));
            if (body.has("price"))        car.setPrice(getDouble(body, "price"));
            if (body.has("mileageKm"))    car.setMileage(getInt(body, "mileageKm"));
            if (body.has("color"))        car.setColor(getString(body, "color"));
            if (body.has("vin"))          car.setVin(getString(body, "vin"));
            if (body.has("imageDataUrl")) car.setImageDataUrl(getString(body, "imageDataUrl"));

            if (!db.updateCar(id, car)) {
                crow::json::wvalue error;
                error["error"] = "Failed to update car";
                return crow::response(500, error);
            }

            Car updated = db.getCarById(id, found);
            crow::json::wvalue response;
            response["id"]           = updated.getCarId();
            response["make"]         = updated.getMake();
            response["model"]        = updated.getModel();
            response["year"]         = updated.getYear();
            response["price"]        = updated.getPrice();
            response["mileageKm"]    = updated.getMileage();
            response["color"]        = updated.getColor();
            response["vin"]          = updated.getVin();
            response["imageDataUrl"] = updated.getImageDataUrl();
            return crow::response(200, response);
        });

        // Delete 
        CROW_ROUTE(app, "/api/cars/<int>").methods("DELETE"_method)
        ([&db](int id) {
          if (!db.carExists(id)) {
             return crow::response(204);  
          }
         if (!db.deleteCar(id)) {
              crow::json::wvalue error;
              error["error"] = "Failed to delete car";
           return crow::response(500, error);
         }
         return crow::response(204);
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
        ([](int) {
            auto res = crow::response(204);
            res.add_header("Allow", "GET, PUT, PATCH, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return res;
        });
    }
};