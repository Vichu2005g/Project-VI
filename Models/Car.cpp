#include "Car.h"

Car::Car() : year(0) {}

Car::Car(std::string make, std::string model, int year)
    : make(std::move(make)), model(std::move(model)), year(year) {}

int Car::getCarId() const {
    return carId;
}

void Car::setCarId(int carId) {
    this->carId = carId;
}

std::string Car::getMake() const {
    return make;
}

void Car::setMake(const std::string& make) {
    this->make = make;
}

std::string Car::getModel() const {
    return model;
}

void Car::setModel(const std::string& model) {
    this->model = model;
}

int Car::getYear() const {
    return year;
}

void Car::setYear(int year) {
    this->year = year;
}

void Car::setVinId(int vinId) {
    this->vinId = vinId;
}

void Car::setMileage(int mileage) {
    this->mileage = mileage;
}
