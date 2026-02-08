const API_URL = 'http://localhost:8080/api/cars';
let editingCarId = null;
let currentImageDataUrl = '';
let allCars = []; 

// Utility functions for normalizing text input
function normalizeText(text) {
    if (!text) return '';
    // Convert to Title Case 
    return text.trim()
        .toLowerCase()
        .split(' ')
        .map(word => word.charAt(0).toUpperCase() + word.slice(1))
        .join(' ');
}

function normalizeUpperCase(text) {
    if (!text) return '';
    return text.trim().toUpperCase();
}

document.addEventListener('DOMContentLoaded', function() {
    loadCars();

    document.getElementById('car-form').addEventListener('submit', handleFormSubmit);
    document.getElementById('cancel-btn').addEventListener('click', resetForm);
    document.getElementById('refresh-btn').addEventListener('click', loadCars);
    document.getElementById('image').addEventListener('change', handleImageSelected);
    
    // Filters and sorts listeners
    document.getElementById('filter-make').addEventListener('change', applyFiltersAndSort);
    document.getElementById('filter-model').addEventListener('change', applyFiltersAndSort);
    document.getElementById('filter-color').addEventListener('change', applyFiltersAndSort);
    document.getElementById('sort-by').addEventListener('change', applyFiltersAndSort);
    document.getElementById('clear-filters').addEventListener('click', clearFilters);
    
    // Modal listeners
    const modal = document.getElementById('car-modal');
    const closeBtn = document.getElementsByClassName('close')[0];
    
    closeBtn.onclick = function() {
        modal.style.display = 'none';
    };
    
    window.onclick = function(event) {
        if (event.target == modal) {
            modal.style.display = 'none';
        }
    };
    
    // Close modal on Escape key
    document.addEventListener('keydown', function(event) {
        if (event.key === 'Escape') {
            modal.style.display = 'none';
        }
    });
});

function handleImageSelected(e) {
    const file = e.target.files && e.target.files[0];
    const preview = document.getElementById('image-preview');

    if (!file) {
        currentImageDataUrl = '';
        preview.style.display = 'none';
        preview.src = '';
        return;
    }

    // Check file size and limits it to 5mb
    if (file.size > 5 * 1024 * 1024) {
        showError('Image size should be less than 5MB');
        e.target.value = '';
        return;
    }

    const reader = new FileReader();
    reader.onload = function() {
        currentImageDataUrl = reader.result;
        preview.src = currentImageDataUrl;
        preview.style.display = 'block';
    };
    reader.readAsDataURL(file);
}

async function loadCars() {
    showLoading(true);
    hideError();

    try {
        const response = await fetch(API_URL);
        if (!response.ok) throw new Error('Failed to load cars');
        const cars = await response.json();
        allCars = cars; // Store all cars
        populateFilterOptions(cars);
        applyFiltersAndSort();
    } catch (error) {
        showError('Error loading cars: ' + error.message);
        allCars = [];
        displayCars([]);
    } finally {
        showLoading(false);
    }
}

function populateFilterOptions(cars) {
    // Gets unique values and then sorts them
    const makes = [...new Set(cars.map(car => car.make).filter(Boolean))].sort();
    const models = [...new Set(cars.map(car => car.model).filter(Boolean))].sort();
    const colors = [...new Set(cars.map(car => car.color).filter(Boolean))].sort();
    
    // This populates the Make dropdown
    const makeSelect = document.getElementById('filter-make');
    const currentMake = makeSelect.value;
    makeSelect.innerHTML = '<option value="">All Makes</option>';
    makes.forEach(make => {
        const option = document.createElement('option');
        option.value = make;
        option.textContent = make;
        makeSelect.appendChild(option);
    });
    makeSelect.value = currentMake;
    
    // This populates the Model dropdown
    const modelSelect = document.getElementById('filter-model');
    const currentModel = modelSelect.value;
    modelSelect.innerHTML = '<option value="">All Models</option>';
    models.forEach(model => {
        const option = document.createElement('option');
        option.value = model;
        option.textContent = model;
        modelSelect.appendChild(option);
    });
    modelSelect.value = currentModel;
    
    // Populates the Color dropdown
    const colorSelect = document.getElementById('filter-color');
    const currentColor = colorSelect.value;
    colorSelect.innerHTML = '<option value="">All Colors</option>';
    colors.forEach(color => {
        const option = document.createElement('option');
        option.value = color;
        option.textContent = color;
        colorSelect.appendChild(option);
    });
    colorSelect.value = currentColor;
}

function applyFiltersAndSort() {
    let filteredCars = [...allCars];
    
    // Apply filters
    const makeFilter = document.getElementById('filter-make').value;
    const modelFilter = document.getElementById('filter-model').value;
    const colorFilter = document.getElementById('filter-color').value;
    
    if (makeFilter) {
        filteredCars = filteredCars.filter(car => car.make === makeFilter);
    }
    if (modelFilter) {
        filteredCars = filteredCars.filter(car => car.model === modelFilter);
    }
    if (colorFilter) {
        filteredCars = filteredCars.filter(car => car.color === colorFilter);
    }
    
    // Applies sorting
    const sortBy = document.getElementById('sort-by').value;
    
    switch(sortBy) {
        case 'price-asc':
            filteredCars.sort((a, b) => a.price - b.price);
            break;
        case 'price-desc':
            filteredCars.sort((a, b) => b.price - a.price);
            break;
        case 'year-asc':
            filteredCars.sort((a, b) => a.year - b.year);
            break;
        case 'year-desc':
            filteredCars.sort((a, b) => b.year - a.year);
            break;
        case 'mileage-asc':
            filteredCars.sort((a, b) => a.mileageKm - b.mileageKm);
            break;
        case 'mileage-desc':
            filteredCars.sort((a, b) => b.mileageKm - a.mileageKm);
            break;
    }
    
    displayCars(filteredCars);
}

function clearFilters() {
    document.getElementById('filter-make').value = '';
    document.getElementById('filter-model').value = '';
    document.getElementById('filter-color').value = '';
    document.getElementById('sort-by').value = '';
    applyFiltersAndSort();
}

function displayCars(cars) {
    const container = document.getElementById('cars-container');

    if (!cars || cars.length === 0) {
        container.innerHTML = '<p class="no-cars">No cars match your filters. Try adjusting your search criteria or add a new car listing.</p>';
        return;
    }

    container.innerHTML = cars.map(car => `
        <div class="car-card" onclick="viewCarDetails(${car.id})">
            <div class="car-header">
                <h3>${escapeHtml(car.make)} ${escapeHtml(car.model)}</h3>
                <span class="car-year">${car.year}</span>
            </div>

            <div class="car-details">
                <p><strong> Price:</strong> $${Number(car.price).toLocaleString()}</p>
                <p><strong> Mileage:</strong> ${Number(car.mileageKm).toLocaleString()} km</p>
                ${car.color ? `<p><strong> Color:</strong> ${escapeHtml(car.color)}</p>` : ''}
                ${car.imageDataUrl ? `<img src="${car.imageDataUrl}" alt="Car image" style="margin-top:10px; width:100%; max-height:180px; object-fit:cover; border-radius:8px; border:2px solid #eee;">` : ''}
            </div>

            <div class="car-actions" onclick="event.stopPropagation()">
                <button class="btn btn-edit" onclick="editCar(${car.id})"> Edit</button>
                <button class="btn btn-delete" onclick="deleteCar(${car.id})"> Delete</button>
            </div>
        </div>
    `).join('');
}

async function viewCarDetails(id) {
    try {
        const response = await fetch(`${API_URL}/${id}`);
        if (!response.ok) throw new Error('Failed to load car details');
        const car = await response.json();
        
        showCarModal(car);
    } catch (error) {
        showError('Error loading car details: ' + error.message);
    }
}

function showCarModal(car) {
    const modal = document.getElementById('car-modal');
    const modalBody = document.getElementById('modal-body');
    
    const createdDate = car.createdAt ? new Date(car.createdAt).toLocaleDateString() : 'N/A';
    const updatedDate = car.updatedAt ? new Date(car.updatedAt).toLocaleDateString() : 'N/A';
    
    modalBody.innerHTML = `
        <div class="modal-header">
            <h2>${escapeHtml(car.make)} ${escapeHtml(car.model)}</h2>
            <span class="modal-year">${car.year}</span>
        </div>
        
        ${car.imageDataUrl ? `<img src="${car.imageDataUrl}" alt="${escapeHtml(car.make)} ${escapeHtml(car.model)}" class="modal-image">` : '<div style="text-align:center; padding:40px; background:#f8f9fa; border-radius:8px; margin-bottom:20px; color:#999;">📷 No image available</div>'}
        
        <div class="modal-details">
            <div class="detail-item">
                <strong> Price</strong>
                <span>$${Number(car.price).toLocaleString()}</span>
            </div>
            <div class="detail-item">
                <strong> Mileage</strong>
                <span>${Number(car.mileageKm).toLocaleString()} km</span>
            </div>
            ${car.color ? `
            <div class="detail-item">
                <strong> Color</strong>
                <span>${escapeHtml(car.color)}</span>
            </div>
            ` : ''}
            ${car.vin ? `
            <div class="detail-item">
                <strong> VIN</strong>
                <span>${escapeHtml(car.vin)}</span>
            </div>
            ` : ''}
            <div class="detail-item">
                <strong> Listed</strong>
                <span>${createdDate}</span>
            </div>
            <div class="detail-item">
                <strong> Updated</strong>
                <span>${updatedDate}</span>
            </div>
        </div>
        
        <div class="modal-actions">
            <button class="btn btn-edit" onclick="editCarFromModal(${car.id})"> Edit Listing</button>
            <button class="btn btn-delete" onclick="deleteCarFromModal(${car.id})"> Delete Listing</button>
        </div>
    `;
    
    modal.style.display = 'block';
}

function editCarFromModal(id) {
    document.getElementById('car-modal').style.display = 'none';
    editCar(id);
}

function deleteCarFromModal(id) {
    document.getElementById('car-modal').style.display = 'none';
    deleteCar(id);
}

async function handleFormSubmit(e) {
    e.preventDefault();
    hideError();

    const carData = {
        make: normalizeText(document.getElementById('make').value),        
        model: normalizeText(document.getElementById('model').value),      
        year: parseInt(document.getElementById('year').value),
        price: parseFloat(document.getElementById('price').value),
        mileageKm: parseInt(document.getElementById('mileage').value),
        color: normalizeText(document.getElementById('color').value),     
        vin: normalizeUpperCase(document.getElementById('vin').value),     
        imageDataUrl: currentImageDataUrl || null
    };

    // Basic validation
    if (!carData.make || !carData.model) {
        showError('Make and Model are required');
        return;
    }

    if (carData.year < 1886 || carData.year > 2027) {
        showError('Please enter a valid year');
        return;
    }

    if (carData.price < 0) {
        showError('Price cannot be negative');
        return;
    }

    try {
        let response;

        if (editingCarId) {
            response = await fetch(`${API_URL}/${editingCarId}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(carData)
            });
        } else {
            response = await fetch(API_URL, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(carData)
            });
        }

        if (!response.ok) {
            const errorData = await response.json();
            throw new Error(errorData.error || 'Failed to save car');
        }

        resetForm();
        await loadCars();
        showSuccess(editingCarId ? ' Car updated successfully' : ' Car listing added successfully');
        
        // Scroll to the top to see the new/updated listing
        window.scrollTo({ top: 0, behavior: 'smooth' });
    } catch (error) {
        showError('Error saving car: ' + error.message);
    }
}

async function editCar(id) {
    try {
        const response = await fetch(`${API_URL}/${id}`);
        if (!response.ok) throw new Error('Failed to load car');
        const car = await response.json();

        document.getElementById('make').value = car.make;
        document.getElementById('model').value = car.model;
        document.getElementById('year').value = car.year;
        document.getElementById('price').value = car.price;
        document.getElementById('mileage').value = car.mileageKm;
        document.getElementById('color').value = car.color || '';
        document.getElementById('vin').value = car.vin || '';

        currentImageDataUrl = car.imageDataUrl || '';
        const preview = document.getElementById('image-preview');
        if (currentImageDataUrl) {
            preview.src = currentImageDataUrl;
            preview.style.display = 'block';
        } else {
            preview.src = '';
            preview.style.display = 'none';
        }
        document.getElementById('image').value = '';

        document.getElementById('form-title').textContent = ' Edit Car Listing';
        editingCarId = id;

        document.querySelector('.form-section').scrollIntoView({ behavior: 'smooth' });
    } catch (error) {
        showError('Error loading car: ' + error.message);
    }
}

async function deleteCar(id) {
    if (!confirm(' Are you sure you want to delete this car listing?')) return;

    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');
        await loadCars();
        showSuccess(' Car listing deleted successfully');
    } catch (error) {
        showError('Error deleting car: ' + error.message);
    }
}

function resetForm() {
    document.getElementById('car-form').reset();
    document.getElementById('form-title').textContent = 'Add New Car';
    editingCarId = null;

    currentImageDataUrl = '';
    const preview = document.getElementById('image-preview');
    preview.src = '';
    preview.style.display = 'none';
    document.getElementById('image').value = '';
}

function showLoading(show) {
    document.getElementById('loading').style.display = show ? 'block' : 'none';
}

function showError(message) {
    const errorDiv = document.getElementById('error-message');
    errorDiv.textContent = message;
    errorDiv.className = 'error-message';
    errorDiv.style.display = 'block';
    
    // hides after 5 seconds
    setTimeout(() => {
        hideError();
    }, 5000);
}

function hideError() {
    document.getElementById('error-message').style.display = 'none';
}

function showSuccess(message) {
    const msgDiv = document.getElementById('error-message');
    msgDiv.textContent = message;
    msgDiv.className = 'success-message';
    msgDiv.style.display = 'block';

    setTimeout(() => {
        msgDiv.style.display = 'none';
        msgDiv.className = 'error-message';
    }, 3000);
}

// Utility function 
function escapeHtml(text) {
    const map = {
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#039;'
    };
    return text ? text.replace(/[&<>"']/g, m => map[m]) : '';
}