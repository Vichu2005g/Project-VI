#pragma once
#include <string>
#include <vector>
#include <optional>
#include "Car.h"
#include "sqlite3.h"

class CarRepository {
public:
    CarRepository(const std::string& dbPath = "cars.db");
    ~CarRepository();

    // Initialize database and create tables
    bool initDatabase();

    // CRUD operations
    bool createCar(Car& car);  // Returns true and sets car.carId on success
    std::vector<Car> getAllCars();
    std::optional<Car> getCarById(int id);
    bool updateCar(const Car& car);
    bool deleteCar(int id);

private:
    sqlite3* db;
    std::string dbPath;

    // Helper methods
    bool executeSQL(const std::string& sql);
    Car createCarFromStatement(sqlite3_stmt* stmt);
};
