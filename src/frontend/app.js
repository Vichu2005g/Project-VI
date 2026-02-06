// API's base URL
const API_URL = 'http://localhost:8080/api/cars';

// Current editing car ID
let editingCarId = null;

// Loads cars when the page loads
document.addEventListener('DOMContentLoaded', function() {
    loadCars();
    
    
    document.getElementById('car-form').addEventListener('submit', handleFormSubmit);
    // Cancel 
    document.getElementById('cancel-btn').addEventListener('click', resetForm); 
    // Refresh button
    document.getElementById('refresh-btn').addEventListener('click', loadCars);
});

// Load all the cars from the API
async function loadCars() {
    showLoading(true);
    hideError();
    
    try {
        const response = await fetch(API_URL);
        
        if (!response.ok) {
            throw new Error('Failed to load cars');
        }
        
        const cars = await response.json();
        displayCars(cars);
    } catch (error) {
        showError('Error loading cars: ' + error.message);
    } finally {
        showLoading(false);
    }
}

// Display the cars in the grid
function displayCars(cars) {
    const container = document.getElementById('cars-container');
    
    if (!cars || cars.length === 0) {
        container.innerHTML = '<p class="no-cars">No cars in inventory. Add your first car</p>';
        return;
    }
    
    container.innerHTML = cars.map(car => `
        <div class="car-card">
            <div class="car-header">
                <h3>${car.make} ${car.model}</h3>
                <span class="car-year">${car.year}</span>
            </div>
            <div class="car-details">
                <p><strong>Price:</strong> $${car.price.toLocaleString()}</p>
                <p><strong>Mileage:</strong> ${car.mileageKm.toLocaleString()} km</p>
                ${car.color ? `<p><strong>Color:</strong> ${car.color}</p>` : ''}
                ${car.vin ? `<p><strong>VIN:</strong> ${car.vin}</p>` : ''}
            </div>
            <div class="car-actions">
                <button class="btn btn-edit" onclick="editCar(${car.id})">✏️ Edit</button>
                <button class="btn btn-delete" onclick="deleteCar(${car.id})">🗑️ Delete</button>
            </div>
        </div>
    `).join('');
}

// Handle form submission for adding/editing a car
async function handleFormSubmit(e) {
    e.preventDefault();
    hideError();
    
    const carData = {
        make: document.getElementById('make').value.trim(),
        model: document.getElementById('model').value.trim(),
        year: parseInt(document.getElementById('year').value),
        price: parseFloat(document.getElementById('price').value),
        mileageKm: parseInt(document.getElementById('mileage').value),
        color: document.getElementById('color').value.trim() || null,
        vin: document.getElementById('vin').value.trim() || null
    };
    
    try {
        let response;
        
        if (editingCarId) {
            // Update an existing car 
            response = await fetch(`${API_URL}/${editingCarId}`, {
                method: 'PUT',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(carData)
            });
        } else {
            // Create new car 
            response = await fetch(API_URL, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(carData)
            });
        }
        
        if (!response.ok) {
            const errorData = await response.json();
            throw new Error(errorData.error || 'Failed to save car');
        }
        
        resetForm();
        loadCars();
        showSuccess(editingCarId ? 'Car updated successfully' : 'Car added successfully');
    } catch (error) {
        showError('Error saving car: ' + error.message);
    }
}

// Edit car load data into the form
async function editCar(id) {
    try {
        const response = await fetch(`${API_URL}/${id}`);
        
        if (!response.ok) {
            throw new Error('Failed to load car');
        }
        
        const car = await response.json();
        
        // Fill the form with car data
        document.getElementById('make').value = car.make;
        document.getElementById('model').value = car.model;
        document.getElementById('year').value = car.year;
        document.getElementById('price').value = car.price;
        document.getElementById('mileage').value = car.mileageKm;
        document.getElementById('color').value = car.color || '';
        document.getElementById('vin').value = car.vin || '';
        
        // Update form title and then set editing mode
        document.getElementById('form-title').textContent = 'Edit Car';
        editingCarId = id;
        
        // Scroll to form
        document.querySelector('.form-section').scrollIntoView({ behavior: 'smooth' });
    } catch (error) {
        showError('Error loading car: ' + error.message);
    }
}

// Delete car 
async function deleteCar(id) {
    if (!confirm('Are you sure you want to delete this car?')) {
        return;
    }
    
    try {
        const response = await fetch(`${API_URL}/${id}`, {
            method: 'DELETE'
        });
        
        if (!response.ok) {
            throw new Error('Failed to delete car');
        }
        
        loadCars();
        showSuccess('Car deleted successfully!');
    } catch (error) {
        showError('Error deleting car: ' + error.message);
    }
}

// Reset form to the initial state
function resetForm() {
    document.getElementById('car-form').reset();
    document.getElementById('form-title').textContent = 'Add New Car';
    editingCarId = null;
}

// Show loading signal
function showLoading(show) {
    document.getElementById('loading').style.display = show ? 'block' : 'none';
}

// Error message
function showError(message) {
    const errorDiv = document.getElementById('error-message');
    errorDiv.textContent = message;
    errorDiv.style.display = 'block';
}

// Hide error message
function hideError() {
    document.getElementById('error-message').style.display = 'none';
}

// Show success message
function showSuccess(message) {
    const errorDiv = document.getElementById('error-message');
    errorDiv.textContent = message;
    errorDiv.className = 'success-message';
    errorDiv.style.display = 'block';
    
    setTimeout(() => {
        errorDiv.style.display = 'none';
        errorDiv.className = 'error-message';
    }, 3000);
}