const API_URL = 'http://localhost:8080/api/cars';

function escapeHtml(text) {
    const map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;' };
    return text ? text.replace(/[&<>"']/g, m => map[m]) : '';
}

document.addEventListener('DOMContentLoaded', function () {
    loadCars();
    document.getElementById('refresh-btn').addEventListener('click', loadCars);
});

async function loadCars() {
    showLoading(true);
    hideMessage();
    try {
        const response = await fetch(`${API_URL}?limit=5`);
        if (!response.ok) throw new Error('Failed to load cars');
        const data = await response.json();
        const cars = data.cars || data; 
        displayCars(cars);
    } catch (error) {
        showError('Error loading cars: ' + error.message);
    } finally {
        showLoading(false);
    }
}

function displayCars(cars) {
    const container = document.getElementById('cars-container');
    if (!cars || cars.length === 0) {
        container.innerHTML = '<p class="no-cars">No cars in inventory yet. <a href="add-car.html">Add one now</a>!</p>';
        return;
    }
    container.innerHTML = cars.map(car => `
        <div class="car-card">
            <div class="car-header">
                <h3>${escapeHtml(car.make)} ${escapeHtml(car.model)}</h3>
                <span class="car-year">${car.year}</span>
            </div>
            <div class="car-details">
                <p><strong>Price:</strong> $${Number(car.price).toLocaleString()}</p>
                <p><strong>Mileage:</strong> ${Number(car.mileageKm).toLocaleString()} km</p>
                ${car.color ? `<p><strong>Color:</strong> ${escapeHtml(car.color)}</p>` : ''}
                ${car.imageDataUrl ? `<img src="${car.imageDataUrl}" alt="Car image" style="margin-top:10px; width:100%; max-height:180px; object-fit:cover; border-radius:8px; border:2px solid #eee;">` : ''}
            </div>
            <div class="car-actions" onclick="event.stopPropagation()">
                <button class="btn btn-edit" onclick="window.location.href='add-car.html?id=${car.id}'">Edit</button>
                <button class="btn btn-patch" onclick="openQuickEdit(event, ${car.id}, ${car.price}, '${car.color}')">Quick Edit</button>
                <button class="btn btn-delete" onclick="deleteCar(${car.id})">Delete</button>
            </div>
        </div>
    `).join('');
}

async function deleteCar(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');
        showSuccess('Car listing deleted successfully');
        await loadCars();
    } catch (error) {
        showError('Error deleting car: ' + error.message);
    }
}

function showLoading(show) {
    document.getElementById('loading').style.display = show ? 'block' : 'none';
}

function showError(message) {
    const div = document.getElementById('error-message');
    div.textContent = message;
    div.className = 'error-message';
    div.style.display = 'block';
    setTimeout(() => hideMessage(), 5000);
}

function showSuccess(message) {
    const div = document.getElementById('error-message');
    div.textContent = message;
    div.className = 'success-message';
    div.style.display = 'block';
    setTimeout(() => { div.style.display = 'none'; div.className = 'error-message'; }, 3000);
}

function hideMessage() {
    document.getElementById('error-message').style.display = 'none';
}

function openQuickEdit(event, carId, currentPrice, currentColor) {
    event.stopPropagation();
    document.getElementById('patch-car-id').value  = carId;
    document.getElementById('patch-price').value   = currentPrice;
    document.getElementById('patch-color').value   = currentColor;
    updatePatchPreview();
    document.getElementById('quick-edit-modal').style.display = 'block';
    document.getElementById('patch-price').addEventListener(
        'input', updatePatchPreview
    );
    document.getElementById('patch-color').addEventListener(
        'input', updatePatchPreview
    );
}

function updatePatchPreview() {
    const price = document.getElementById('patch-price').value;
    const color = document.getElementById('patch-color').value;
    document.getElementById('patch-preview').textContent =
        `{ "price": ${price}, "color": "${color}" }`;
}

function closeQuickEdit() {
    document.getElementById('quick-edit-modal').style.display = 'none';
}

async function submitPatch() {
    const id    = document.getElementById('patch-car-id').value;
    const price = parseFloat(document.getElementById('patch-price').value);
    const color = document.getElementById('patch-color').value.trim();

    if (!price || price < 0) { alert('Please enter a valid price'); return; }
    if (!color) { alert('Please enter a color'); return; }

    const patchData = { price: price, color: color };

    try {
        const response = await fetch(`${API_URL}/${id}`, {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(patchData)
        });

        if (!response.ok) {
            const err = await response.json();
            throw new Error(err.error || 'Failed to patch car');
        }

        const updated = await response.json();
        closeQuickEdit();
        showSuccess(
            `Quick Edit applied — Price: $${Number(updated.price)
            .toLocaleString()}, Color: ${updated.color}`
        );
        await loadCars(); 

    } catch (error) {
        alert('Error applying quick edit: ' + error.message);
    }
}

document.getElementById('quick-edit-modal')
    .addEventListener('click', function(e) {
        if (e.target === this) closeQuickEdit();
    });