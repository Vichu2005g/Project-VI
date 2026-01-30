#include "CarRepository.h"
#include <iostream>

CarRepository::CarRepository(const std::string& dbPath) 
    : db(nullptr), dbPath(dbPath) {
}

CarRepository::~CarRepository() {
    if (db) {
        sqlite3_close(db);
    }
}

bool CarRepository::initDatabase() {
    // Open database
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Create cars table
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS cars (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            make TEXT NOT NULL,
            model TEXT NOT NULL,
            year INTEGER NOT NULL,
            vin_id INTEGER,
            mileage INTEGER
        );
    )";

    return executeSQL(sql);
}

bool CarRepository::createCar(Car& car) {
    const char* sql = R"(
        INSERT INTO cars (make, model, year, vin_id, mileage)
        VALUES (?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, car.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, car.getYear());
    sqlite3_bind_int(stmt, 4, car.getVinId());
    sqlite3_bind_int(stmt, 5, car.getMileage());

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert car: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Set the auto-generated ID
    car.setCarId(static_cast<int>(sqlite3_last_insert_rowid(db)));
    return true;
}

std::vector<Car> CarRepository::getAllCars() {
    std::vector<Car> cars;
    const char* sql = "SELECT id, make, model, year, vin_id, mileage FROM cars;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return cars;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cars.push_back(createCarFromStatement(stmt));
    }

    sqlite3_finalize(stmt);
    return cars;
}

std::optional<Car> CarRepository::getCarById(int id) {
    const char* sql = "SELECT id, make, model, year, vin_id, mileage FROM cars WHERE id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Car car = createCarFromStatement(stmt);
        sqlite3_finalize(stmt);
        return car;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}

bool CarRepository::updateCar(const Car& car) {
    const char* sql = R"(
        UPDATE cars 
        SET make = ?, model = ?, year = ?, vin_id = ?, mileage = ?
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, car.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, car.getYear());
    sqlite3_bind_int(stmt, 4, car.getVinId());
    sqlite3_bind_int(stmt, 5, car.getMileage());
    sqlite3_bind_int(stmt, 6, car.getCarId());

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to update car: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return sqlite3_changes(db) > 0;
}

bool CarRepository::deleteCar(int id) {
    const char* sql = "DELETE FROM cars WHERE id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to delete car: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return sqlite3_changes(db) > 0;
}

bool CarRepository::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

Car CarRepository::createCarFromStatement(sqlite3_stmt* stmt) {
    Car car;
    car.setCarId(sqlite3_column_int(stmt, 0));
    car.setMake(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    car.setModel(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
    car.setYear(sqlite3_column_int(stmt, 3));
    car.setVinId(sqlite3_column_int(stmt, 4));
    car.setMileage(sqlite3_column_int(stmt, 5));
    return car;
}
