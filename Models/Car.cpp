#include "Car.h"


void Car::touch(){
    updatedAt = std::chrono::system_clock::now();   
}

Car::Car(std::string make, std::string model, std::string colour,
         int year, int vinId, int mileage)
    : make(std::move(make)),
      model(std::move(model)),
      colour(std::move(colour)),
      year(year),
      vinId(vinId),
      mileage(mileage)
{
    createdAt = std::chrono::system_clock::now();
    updatedAt = createdAt;
}

std::string Car::getMake() const {
    return make;
}

void Car::setMake(const std::string& make) {
    this->make = make;
    touch();
}

std::string Car::getModel() const {
    return model;
}

void Car::setModel(const std::string& model) {
    this->model = model;
    touch();
}

int Car::getYear() const {
    return year;
}

void Car::setYear(int year) {
    this->year = year;
    touch();
}

void Car::setVinId(int vinId) {
    this->vinId = vinId;
    touch();
}

int Car::getVinId() const {
    return vinId;
}

int Car::getMileage() const {
    return mileage;
}

void Car::setMileage(int mileage) {
    this->mileage = mileage;
    touch();
}

void Car::setColour(const std::string& colour) {
    this->colour = colour;
    touch();
}

std::string Car::getColour() const {
    return colour;
}

