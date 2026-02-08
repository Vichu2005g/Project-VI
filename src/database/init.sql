-- Database Schema

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

-- sample data 
INSERT OR IGNORE INTO cars (make, model, year, price, mileage_km, color, vin, image_data_url, created_at, updated_at)
VALUES
    ('Toyota', 'Corolla', 2020, 21999.99, 45000, 'Blue', '2HGFC2F69LH000001', NULL, datetime('now'), datetime('now')),
    ('Honda', 'Civic', 2021, 23999.99, 32000, 'Red', '2HGFC2F69LH000002', NULL, datetime('now'), datetime('now'));

SELECT 'Database up and running' as message;
SELECT COUNT(*) as total_cars FROM cars;
