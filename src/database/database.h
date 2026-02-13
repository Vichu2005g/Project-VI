#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include "../../Models/Car.h"

class Database {
public:
    // Constructor and Destructor
    Database(const std::string& dbPath);
    ~Database();

    // Initialize the  database and create the tables...well a single table so far
    bool initialize();

    // CRUD Operations
    bool insertCar(const Car& car, int& newId);
    bool updateCar(int id, const Car& car);
    bool deleteCar(int id);
    Car getCarById(int id, bool& found);
    std::vector<Car> getAllCars();

    // Utility methods
    bool carExists(int id);
    bool vinExists(const std::string& vin);
    void close();

private:
    sqlite3* db;
    std::string dbPath;
    
    // Helper function to run SQL
    bool executeSQL(const std::string& sql);
};