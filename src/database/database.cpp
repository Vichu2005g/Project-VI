#include "database.h"
#include <iostream>
#include <ctime>

// Constructor
Database::Database(const std::string& dbPath) : db(nullptr), dbPath(dbPath) {}

// Destructor
Database::~Database() { close(); }

// Helper function to get current timestamp
static std::string getCurrentTimestamp() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now); 
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buf;
}

bool Database::initialize() {
    int result = sqlite3_open(dbPath.c_str(), &db);

    if (result != SQLITE_OK) {
        std::cerr << "Failed to open the database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::cout << "Database opened successfully: " << dbPath << std::endl;

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
            image_data_url TEXT,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL
        );

        CREATE INDEX IF NOT EXISTS idx_cars_make_model ON cars(make, model);
        CREATE INDEX IF NOT EXISTS idx_cars_year ON cars(year);
        CREATE UNIQUE INDEX IF NOT EXISTS idx_cars_vin ON cars(vin) WHERE vin IS NOT NULL;
    )";

    return executeSQL(createTableSQL);
}

// Insert
bool Database::insertCar(const Car& car, int& newId) {
    std::string timestamp = getCurrentTimestamp();

    std::string sql =
        "INSERT INTO cars (make, model, year, price, mileage_km, color, vin, image_data_url, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, car.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, car.getYear());

    sqlite3_bind_double(stmt, 4, car.getPrice());
    sqlite3_bind_int(stmt, 5, car.getMileage());

    // color 
    if (car.getColor().empty()) sqlite3_bind_null(stmt, 6);
    else sqlite3_bind_text(stmt, 6, car.getColor().c_str(), -1, SQLITE_TRANSIENT);

    // vin 
    if (car.getVin().empty()) sqlite3_bind_null(stmt, 7);
    else sqlite3_bind_text(stmt, 7, car.getVin().c_str(), -1, SQLITE_TRANSIENT);

    // image_data_url 
    if (car.getImageDataUrl().empty()) sqlite3_bind_null(stmt, 8);
    else sqlite3_bind_text(stmt, 8, car.getImageDataUrl().c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 9, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, timestamp.c_str(), -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to insert car: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    newId = static_cast<int>(sqlite3_last_insert_rowid(db));
    sqlite3_finalize(stmt);

    return true;
}

// Update
bool Database::updateCar(int id, const Car& car) {
    std::string timestamp = getCurrentTimestamp();

    std::string sql =
        "UPDATE cars SET make = ?, model = ?, year = ?, price = ?, mileage_km = ?, color = ?, vin = ?, image_data_url = ?, updated_at = ? "
        "WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, car.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, car.getYear());
    sqlite3_bind_double(stmt, 4, car.getPrice());
    sqlite3_bind_int(stmt, 5, car.getMileage());

    if (car.getColor().empty()) sqlite3_bind_null(stmt, 6);
    else sqlite3_bind_text(stmt, 6, car.getColor().c_str(), -1, SQLITE_TRANSIENT);

    if (car.getVin().empty()) sqlite3_bind_null(stmt, 7);
    else sqlite3_bind_text(stmt, 7, car.getVin().c_str(), -1, SQLITE_TRANSIENT);

    if (car.getImageDataUrl().empty()) sqlite3_bind_null(stmt, 8);
    else sqlite3_bind_text(stmt, 8, car.getImageDataUrl().c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 9, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 10, id);

    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to update car: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

// Delete
bool Database::deleteCar(int id) {
    std::string sql = "DELETE FROM cars WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, id);
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return result == SQLITE_DONE;
}

// Get by id
Car Database::getCarById(int id, bool& found) {
    Car car;
    found = false;

    std::string sql =
        "SELECT id, make, model, year, price, mileage_km, color, vin, image_data_url, created_at, updated_at "
        "FROM cars WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) return car;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;

        car.setCarId(sqlite3_column_int(stmt, 0));

        const unsigned char* makeTxt = sqlite3_column_text(stmt, 1);
        const unsigned char* modelTxt = sqlite3_column_text(stmt, 2);

        car.setMake(makeTxt ? reinterpret_cast<const char*>(makeTxt) : "");
        car.setModel(modelTxt ? reinterpret_cast<const char*>(modelTxt) : "");

        car.setYear(sqlite3_column_int(stmt, 3));
        car.setPrice(sqlite3_column_double(stmt, 4));
        car.setMileage(sqlite3_column_int(stmt, 5));

        const unsigned char* colorTxt = sqlite3_column_text(stmt, 6);
        const unsigned char* vinTxt = sqlite3_column_text(stmt, 7);
        const unsigned char* imgTxt = sqlite3_column_text(stmt, 8);
        const unsigned char* createdTxt = sqlite3_column_text(stmt, 9);
        const unsigned char* updatedTxt = sqlite3_column_text(stmt, 10);

        car.setColor(colorTxt ? reinterpret_cast<const char*>(colorTxt) : "");
        car.setVin(vinTxt ? reinterpret_cast<const char*>(vinTxt) : "");
        car.setImageDataUrl(imgTxt ? reinterpret_cast<const char*>(imgTxt) : "");
        car.setCreatedAt(createdTxt ? reinterpret_cast<const char*>(createdTxt) : "");
        car.setUpdatedAt(updatedTxt ? reinterpret_cast<const char*>(updatedTxt) : "");
    }

    sqlite3_finalize(stmt);
    return car;
}

// Get all
std::vector<Car> Database::getAllCars() {
    std::vector<Car> cars;

    std::string sql =
        "SELECT id, make, model, year, price, mileage_km, color, vin, image_data_url, created_at, updated_at "
        "FROM cars ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) return cars;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Car car;

        car.setCarId(sqlite3_column_int(stmt, 0));

        const unsigned char* makeTxt = sqlite3_column_text(stmt, 1);
        const unsigned char* modelTxt = sqlite3_column_text(stmt, 2);
        car.setMake(makeTxt ? reinterpret_cast<const char*>(makeTxt) : "");
        car.setModel(modelTxt ? reinterpret_cast<const char*>(modelTxt) : "");

        car.setYear(sqlite3_column_int(stmt, 3));
        car.setPrice(sqlite3_column_double(stmt, 4));
        car.setMileage(sqlite3_column_int(stmt, 5));

        const unsigned char* colorTxt = sqlite3_column_text(stmt, 6);
        const unsigned char* vinTxt = sqlite3_column_text(stmt, 7);
        const unsigned char* imgTxt = sqlite3_column_text(stmt, 8);
        const unsigned char* createdTxt = sqlite3_column_text(stmt, 9);
        const unsigned char* updatedTxt = sqlite3_column_text(stmt, 10);

        car.setColor(colorTxt ? reinterpret_cast<const char*>(colorTxt) : "");
        car.setVin(vinTxt ? reinterpret_cast<const char*>(vinTxt) : "");
        car.setImageDataUrl(imgTxt ? reinterpret_cast<const char*>(imgTxt) : "");
        car.setCreatedAt(createdTxt ? reinterpret_cast<const char*>(createdTxt) : "");
        car.setUpdatedAt(updatedTxt ? reinterpret_cast<const char*>(updatedTxt) : "");

        cars.push_back(car);
    }

    sqlite3_finalize(stmt);
    return cars;
}

bool Database::carExists(int id) {
    std::string sql = "SELECT COUNT(*) FROM cars WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, id);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::vinExists(const std::string& vin) {
    if (vin.empty()) return false;

    std::string sql = "SELECT COUNT(*) FROM cars WHERE vin = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::executeSQL(const std::string& sql) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK) {
        std::cerr << "SQL error: " << (errorMessage ? errorMessage : "unknown") << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}
