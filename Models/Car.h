/**
 * @file Car.h
 * @brief Definition of the Car model class.
 */

#pragma once
#include <string>

/**
 * @class Car
 * @brief Represents a car listing in the inventory.
 * 
 * This class holds all necessary details for a car, including its identification,
 * specifications, and metadata like timestamps and image data.
 */
class Car {
public:
    /** @brief Default constructor. */
    Car();
    
    /**
     * @brief Parameterized constructor.
     * @param make The manufacturer of the car.
     * @param model The specific model of the car.
     * @param year The manufacturing year.
     */
    Car(std::string make, std::string model, int year);

    // Getters
    /** @brief Returns the unique database ID of the car. */
    int getCarId() const;
    /** @brief Returns the manufacturer of the car. */
    std::string getMake() const;
    /** @brief Returns the specific model of the car. */
    std::string getModel() const;
    /** @brief Returns the manufacturing year. */
    int getYear() const;
    /** @brief Returns the selling price of the car. */
    double getPrice() const;
    /** @brief Returns the mileage in Kilometers. */
    int getMileage() const;
    /** @brief Returns the color of the car. */
    std::string getColor() const;
    /** @brief Returns the Vehicle Identification Number (VIN). */
    std::string getVin() const;
    /** @brief Returns the Base64 encoded image data URL. */
    std::string getImageDataUrl() const;   
    /** @brief Returns the ISO timestamp when the listing was created. */
    std::string getCreatedAt() const;
    /** @brief Returns the ISO timestamp when the listing was last updated. */
    std::string getUpdatedAt() const;

    // Setters
    /** @brief Sets the unique database ID. */
    void setCarId(int carId);
    /** @brief Sets the manufacturer name. */
    void setMake(const std::string& make);
    /** @brief Sets the model name. */
    void setModel(const std::string& model);
    /** @brief Sets the manufacturing year. */
    void setYear(int year);
    /** @brief Sets the selling price. */
    void setPrice(double price);
    /** @brief Sets the car's mileage. */
    void setMileage(int mileage);
    /** @brief Sets the exterior color. */
    void setColor(const std::string& color);
    /** @brief Sets the individual VIN. */
    void setVin(const std::string& vin);
    /** @brief Sets the image data as a URL or Base64 string. */
    void setImageDataUrl(const std::string& imageDataUrl); 
    /** @brief Sets the creation timestamp. */
    void setCreatedAt(const std::string& createdAt);
    /** @brief Sets the last modification timestamp. */
    void setUpdatedAt(const std::string& updatedAt);

private:
    int carId;              /**< Unique identifier for the car. */
    std::string make;       /**< Car manufacturer. */
    std::string model;      /**< Car model. */
    int year;               /**< Manufacture year. */
    double price;           /**< Sale price. */
    int mileage;            /**< Current mileage in Km. */
    std::string color;      /**< Exterior color. */
    std::string vin;        /**< Vehicle Identification Number. */
    std::string imageDataUrl;/**< Base64 encoded image link/data. */
    std::string createdAt;  /**< Creation timestamp string. */
    std::string updatedAt;  /**< Last update timestamp string. */
};
