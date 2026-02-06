#include "database.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

// Constructor
Database::Database(const std::string& dbPath) : db(nullptr), dbPath(dbPath) {}

// Destructor
Database::~Database() {
    close();
}

// start database and create tables
bool Database::initialize() {
    // Open the database connection
    int result = sqlite3_open(dbPath.c_str(), &db);
    
    if (result != SQLITE_OK) {
        std::cerr << "Failed to open the database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    std::cout << "Database opened successfully: " << dbPath << std::endl;
    
    // Read and execute init.sql
    std::string createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS cars (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            make TEXT NOT NULL,
            model TEXT NOT NULL,
            year INTEGER NOT NULL,
            price REAL NOT NULL,
            mileage_km INTEGER NOT NULL,
            color TEXT,
            vin TEXT UNIQUE,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL
        );
        
        CREATE INDEX IF NOT EXISTS idx_cars_make_model ON cars(make, model);
        CREATE INDEX IF NOT EXISTS idx_cars_year ON cars(year);
        CREATE UNIQUE INDEX IF NOT EXISTS idx_cars_vin ON cars(vin) WHERE vin IS NOT NULL;
    )";
    
    return executeSQL(createTableSQL);
}

// Helper function to get the current timestamp
std::string getCurrentTimestamp() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buf;
}

// Insert a new car
bool Database::insertCar(const Car& car, int& newId) {
    std::string timestamp = getCurrentTimestamp();
    
    std::string sql = "INSERT INTO cars (make, model, year, price, mileage_km, color, vin, created_at, updated_at) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Bind the  parameters
    sqlite3_bind_text(stmt, 1, car.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, car.getYear());
    sqlite3_bind_double(stmt, 4, 0.0); 
    sqlite3_bind_int(stmt, 5, car.getMileage());
    sqlite3_bind_text(stmt, 6, "", -1, SQLITE_TRANSIENT); 
    sqlite3_bind_text(stmt, 7, "", -1, SQLITE_TRANSIENT); 
    sqlite3_bind_text(stmt, 8, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    
    result = sqlite3_step(stmt);
    
    if (result != SQLITE_DONE) {
        std::cerr << "Failed to insert car: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    
    newId = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    std::cout << "Car inserted successfully with ID: " << newId << std::endl;
    return true;
}

// Update an existing car
bool Database::updateCar(int id, const Car& car) {
    std::string timestamp = getCurrentTimestamp();
    
    std::string sql = "UPDATE cars SET make = ?, model = ?, year = ?, mileage_km = ?, updated_at = ? "
                      "WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, car.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, car.getYear());
    sqlite3_bind_int(stmt, 4, car.getMileage());
    sqlite3_bind_text(stmt, 5, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        std::cerr << "Failed to update car: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    std::cout << "Car updated successfully: ID " << id << std::endl;
    return true;
}

// Delete a car
bool Database::deleteCar(int id) {
    std::string sql = "DELETE FROM cars WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        std::cerr << "Failed to delete car: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    std::cout << "Car deleted successfully: ID " << id << std::endl;
    return true;
}

// Get a car by ID
Car Database::getCarById(int id, bool& found) {
    Car car;
    found = false;
    
    std::string sql = "SELECT id, make, model, year, mileage_km FROM cars WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return car;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        
        int carId = sqlite3_column_int(stmt, 0);
        std::string make = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int year = sqlite3_column_int(stmt, 3);
        int mileage = sqlite3_column_int(stmt, 4);
        
        car.setCarId(carId);
        car.setMake(make);
        car.setModel(model);
        car.setYear(year);
        car.setMileage(mileage);
    }
    
    sqlite3_finalize(stmt);
    return car;
}

// Get all the cars
std::vector<Car> Database::getAllCars() {
    std::vector<Car> cars;
    
    std::string sql = "SELECT id, make, model, year, mileage_km FROM cars ORDER BY id;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return cars;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Car car;
        
        int carId = sqlite3_column_int(stmt, 0);
        std::string make = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int year = sqlite3_column_int(stmt, 3);
        int mileage = sqlite3_column_int(stmt, 4);
        
        car.setCarId(carId);
        car.setMake(make);
        car.setModel(model);
        car.setYear(year);
        car.setMileage(mileage);
        
        cars.push_back(car);
    }
    
    sqlite3_finalize(stmt);
    std::cout << "Retrieved " << cars.size() << " cars from database" << std::endl;
    
    return cars;
}

// Check if the car does exists or not
bool Database::carExists(int id) {
    std::string sql = "SELECT COUNT(*) FROM cars WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    bool exists = false;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    
    sqlite3_finalize(stmt);
    return exists;
}

// Check if VIN exists
bool Database::vinExists(const std::string& vin) {
    if (vin.empty()) return false;
    
    std::string sql = "SELECT COUNT(*) FROM cars WHERE vin = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);
    bool exists = false;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    
    sqlite3_finalize(stmt);
    return exists;
}

// Execute SQL query
bool Database::executeSQL(const std::string& sql) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }
    
    std::cout << "SQL executed successfully" << std::endl;
    return true;
}

// Close database connection
void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
        std::cout << "Database connection closed" << std::endl;
    }
}