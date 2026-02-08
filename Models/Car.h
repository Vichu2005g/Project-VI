#pragma once
#include <string>

class Car {
public:
    Car();
    Car(std::string make, std::string model, int year);

    // Getters
    int getCarId() const;
    std::string getMake() const;
    std::string getModel() const;
    int getYear() const;
    double getPrice() const;
    int getMileage() const;
    std::string getColor() const;
    std::string getVin() const;
    std::string getImageDataUrl() const;   
    std::string getCreatedAt() const;
    std::string getUpdatedAt() const;

    // Setters
    void setCarId(int carId);
    void setMake(const std::string& make);
    void setModel(const std::string& model);
    void setYear(int year);
    void setPrice(double price);
    void setMileage(int mileage);
    void setColor(const std::string& color);
    void setVin(const std::string& vin);
    void setImageDataUrl(const std::string& imageDataUrl); 
    void setCreatedAt(const std::string& createdAt);
    void setUpdatedAt(const std::string& updatedAt);

private:
    int carId;
    std::string make;
    std::string model;
    int year;
    double price;
    int mileage;
    std::string color;
    std::string vin;
    std::string imageDataUrl;     
    std::string createdAt;
    std::string updatedAt;
};
