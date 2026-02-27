/**
 * @file database.h
 * @brief SQLite3 database wrapper for Car Inventory persistence.
 */

#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include "../../Models/Car.h"

/**
 * @class Database
 * @brief Handles all direct interactions with the SQLite3 database.
 * 
 * This class provides a high-level API for performing CRUD operations on the
 * car inventory, managing the underlying sqlite3 connection and prepared statements.
 */
class Database {
public:
    /**
     * @brief Constructs a Database object.
     * @param dbPath Path to the SQLite database file.
     */
    Database(const std::string& dbPath);

    /** @brief Destructor that ensures the database connection is closed. */
    ~Database();

    /**
     * @brief Initializes the database and creates the necessary tables.
     * @return true if initialization was successful, false otherwise.
     */
    bool initialize();

    /**
     * @brief Inserts a new car record into the database.
     * @param car The Car object containing data to insert.
     * @param newId Reference to an integer to store the auto-generated primary key.
     * @return true on success.
     */
    bool insertCar(const Car& car, int& newId);

    /**
     * @brief Updates an existing car record.
     * @param id The ID of the car to update.
     * @param car The Car object containing updated data.
     * @return true on success.
     */
    bool updateCar(int id, const Car& car);

    /**
     * @brief Deletes a car record by its ID.
     * @param id The ID of the car to remove.
     * @return true on success.
     */
    bool deleteCar(int id);

    /**
     * @brief Retrieves a single car record by its ID.
     * @param id The ID to search for.
     * @param found Reference to a boolean set to true if the car exists.
     * @return The populated Car object.
     */
    Car getCarById(int id, bool& found);

    /**
     * @brief Retrieves all car records from the database.
     * @param limit Optional limit on the number of records returned (default: all).
     * @return A vector of populated Car objects.
     */
    std::vector<Car> getAllCars(int limit = -1);

    /**
     * @brief Checks if a car with the given ID exists.
     * @param id The ID to check.
     * @return true if exists.
     */
    bool carExists(int id);

    /**
     * @brief Checks if a VIN is already present in the database.
     * @param vin The VIN string to check.
     * @return true if the VIN is already registered.
     */
    bool vinExists(const std::string& vin);

    /** @brief Explicitly closes the database connection. */
    void close();

private:
    sqlite3* db;            /**< Raw pointer to the SQLite3 database connection. */
    std::string dbPath;     /**< Path to the database file. */
    
    /**
     * @brief Executes a simple SQL command without results.
     * @param sql The raw SQL string to execute.
     * @return true on success.
     */
    bool executeSQL(const std::string& sql);
};
