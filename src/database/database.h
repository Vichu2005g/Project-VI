#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include "../../Models/Car.h"

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();

    bool initialize();

    // CRUD Operations
    bool insertCar(const Car& car, int& newId);
    bool updateCar(int id, const Car& car);
    bool deleteCar(int id);
    Car getCarById(int id, bool& found);

    // Updated — now has offset parameter
    std::vector<Car> getAllCars(int limit = -1, int offset = 0);

    // NEW — excludes image data for list views (fast)
    std::vector<Car> getAllCarsSummary(int limit = 20, int offset = 0);

    // NEW — total count for pagination metadata
    int getTotalCarCount();

    bool carExists(int id);
    bool vinExists(const std::string& vin);
    void close();

private:
    sqlite3* db;
    std::string dbPath;
    bool executeSQL(const std::string& sql);
};