#include "Car.h"

// Default constructor
Car::Car() : carId(0), year(0), price(0.0), mileage(0) {}

// Parameterized constructor
Car::Car(std::string make, std::string model, int year)
    : carId(0), make(std::move(make)), model(std::move(model)),
      year(year), price(0.0), mileage(0) {}

// Getters
int Car::getCarId() const { return carId; }
std::string Car::getMake() const { return make; }
std::string Car::getModel() const { return model; }
int Car::getYear() const { return year; }
double Car::getPrice() const { return price; }
int Car::getMileage() const { return mileage; }
std::string Car::getColor() const { return color; }
std::string Car::getVin() const { return vin; }
std::string Car::getImageDataUrl() const { return imageDataUrl; } // ✅ NEW
std::string Car::getCreatedAt() const { return createdAt; }
std::string Car::getUpdatedAt() const { return updatedAt; }

// Setters
void Car::setCarId(int carId) { this->carId = carId; }
void Car::setMake(const std::string& make) { this->make = make; }
void Car::setModel(const std::string& model) { this->model = model; }
void Car::setYear(int year) { this->year = year; }
void Car::setPrice(double price) { this->price = price; }
void Car::setMileage(int mileage) { this->mileage = mileage; }
void Car::setColor(const std::string& color) { this->color = color; }
void Car::setVin(const std::string& vin) { this->vin = vin; }
void Car::setImageDataUrl(const std::string& imageDataUrl) { this->imageDataUrl = imageDataUrl; } // ✅ NEW
void Car::setCreatedAt(const std::string& createdAt) { this->createdAt = createdAt; }
void Car::setUpdatedAt(const std::string& updatedAt) { this->updatedAt = updatedAt; }
