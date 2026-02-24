const API_URL = 'http://localhost:8080/api/cars';
let editingCarId = null;
let currentImageDataUrl = '';

function escapeHtml(text) {
    const map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;' };
    return text ? text.replace(/[&<>"']/g, m => map[m]) : '';
}

function normalizeText(text) {
    if (!text) return '';
    return text.trim().toLowerCase().split(' ')
        .map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(' ');
}

function normalizeUpperCase(text) {
    if (!text) return '';
    return text.trim().toUpperCase();
}

document.addEventListener('DOMContentLoaded', async function () {
    const params = new URLSearchParams(window.location.search);
    const id = params.get('id');
    if (id) {
        editingCarId = parseInt(id);
        await loadCarForEdit(editingCarId);
    }

    document.getElementById('car-form').addEventListener('submit', handleFormSubmit);
    document.getElementById('cancel-btn').addEventListener('click', () => window.location.href = '/');
    document.getElementById('image').addEventListener('change', handleImageSelected);
});

async function loadCarForEdit(id) {
    try {
        const response = await fetch(`${API_URL}/${id}`);
        if (!response.ok) throw new Error('Car not found');
        const car = await response.json();

        document.getElementById('form-title').textContent = 'Edit Car Listing';
        document.title = 'Edit Car - Car Inventory';
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
        }
    } catch (error) {
        showError('Error loading car: ' + error.message);
    }
}

function handleImageSelected(e) {
    const file = e.target.files && e.target.files[0];
    const preview = document.getElementById('image-preview');
    if (!file) {
        currentImageDataUrl = '';
        preview.style.display = 'none';
        preview.src = '';
        return;
    }
    if (file.size > 5 * 1024 * 1024) {
        showError('Image size should be less than 5MB');
        e.target.value = '';
        return;
    }
    const reader = new FileReader();
    reader.onload = function () {
        currentImageDataUrl = reader.result;
        preview.src = currentImageDataUrl;
        preview.style.display = 'block';
    };
    reader.readAsDataURL(file);
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

    if (!carData.make || !carData.model) { showError('Make and Model are required'); return; }
    if (carData.year < 1886 || carData.year > 2027) { showError('Please enter a valid year'); return; }
    if (carData.price < 0) { showError('Price cannot be negative'); return; }

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

        window.location.href = '/';
    } catch (error) {
        showError('Error saving car: ' + error.message);
    }
}

function showError(message) {
    const div = document.getElementById('error-message');
    div.textContent = message;
    div.className = 'error-message';
    div.style.display = 'block';
    setTimeout(() => hideError(), 5000);
}

function hideError() {
    document.getElementById('error-message').style.display = 'none';
}
