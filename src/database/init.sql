# Database Schema


# Create cars table
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

# Query Indexes 
CREATE INDEX IF NOT EXISTS idx_cars_make_model ON cars(make, model);
CREATE INDEX IF NOT EXISTS idx_cars_year ON cars(year);
CREATE UNIQUE INDEX IF NOT EXISTS idx_cars_vin ON cars(vin) WHERE vin IS NOT NULL;

# sample data for testing 
INSERT OR IGNORE INTO cars (make, model, year, price, mileage_km, color, vin, created_at, updated_at) 
VALUES 
    ('Toyota', 'Corolla', 2020, 21999.99, 45000, 'Blue', '2HGFC2F69LH000001', datetime('now'), datetime('now')),
    ('Honda', 'Civic', 2021, 23999.99, 32000, 'Red', '2HGFC2F69LH000002', datetime('now'), datetime('now')),
    ('Ford', 'F-150', 2019, 35999.99, 67000, 'Black', '1FTFW1E84KFA12345', datetime('now'), datetime('now')),
    ('Tesla', 'Model 3', 2022, 45999.99, 12000, 'White', '5YJ3E1EA0KF123456', datetime('now'), datetime('now')),
    ('Chevrolet', 'Malibu', 2020, 24999.99, 38000, 'Silver', '1G1ZD5ST5LF123456', datetime('now'), datetime('now'));

# Display confirmation
SELECT 'Database initialized successfully' as message;
SELECT COUNT(*) as total_cars FROM cars;