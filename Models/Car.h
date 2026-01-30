#pragma once
#include <chrono>
#include <string>

class Car {
public:

    using TimePoint = std::chrono::system_clock::time_point;

    Car();
    Car(std::string make, std::string model, std::string colour, int year, int vinId, int mileage);

    std::string getMake() const;
    void setMake(const std::string& make);

    std::string getModel() const;
    void setModel(const std::string& model);

    std::string getColour() const;
    void setColour(const std::string& colour);

    int getYear() const;
    void setYear(int year);

    int getVinId() const;
    void setVinId(int vinId);

    int getMileage() const;
    void setMileage(int mileage);

    TimePoint getCreatedAt() const;
    TimePoint getUpdatedAt() const;

    void setCreatedAt(TimePoint createdAt);
    void setUpdatedAt(TimePoint updatedAt); 
    
private:
    void touch();
    std::string make;
    std::string model;
    std::string colour;
    int year;
    int vinId;
    int mileage;

};
