#pragma once
#include <string>

class Car {
public:
    Car();
    Car(std::string make, std::string model, int year);

    std::string getMake() const;
    void setMake(const std::string& make);

    std::string getModel() const;
    void setModel(const std::string& model);

    int getYear() const;
    void setYear(int year);

    int getVinId() const;
    void setVinId(int vinId);

    int getMileage() const;
    void setMileage(int mileage);
private:
    std::string make;
    std::string model;
    int year;
    int vinId;
    int mileage;
};
