const API_URL = 'http://localhost:8080/api/cars';
let allCars = [];
const DISPLAY_LIMIT = 10;

function escapeHtml(text) {
    const map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;' };
    return text ? text.replace(/[&<>"']/g, m => map[m]) : '';
}

document.addEventListener('DOMContentLoaded', function () {
    loadCars();

    document.getElementById('refresh-btn').addEventListener('click', loadCars);
    document.getElementById('filter-make').addEventListener('change', applyFiltersAndSort);
    document.getElementById('filter-model').addEventListener('change', applyFiltersAndSort);
    document.getElementById('filter-color').addEventListener('change', applyFiltersAndSort);
    document.getElementById('sort-by').addEventListener('change', applyFiltersAndSort);
    document.getElementById('clear-filters').addEventListener('click', clearFilters);

    const popup = document.getElementById('car-popup');
    document.getElementById('popup-close').onclick = () => popup.style.display = 'none';
    window.onclick = e => { if (e.target === popup) popup.style.display = 'none'; };
    document.addEventListener('keydown', e => { if (e.key === 'Escape') popup.style.display = 'none'; });
});

async function loadCars() {
    showLoading(true);
    hideMessage();
    try {
        const response = await fetch(API_URL);
        if (!response.ok) throw new Error('Failed to load cars');
        allCars = await response.json();
        populateFilterOptions(allCars);
        computeStats(allCars);
        applyFiltersAndSort();
    } catch (error) {
        showError('Error loading cars: ' + error.message);
        allCars = [];
    } finally {
        showLoading(false);
    }
}

function computeStats(cars) {
    document.getElementById('stat-total').textContent = cars.length;

    if (cars.length > 0) {
        const avg = cars.reduce((sum, c) => sum + Number(c.price), 0) / cars.length;
        document.getElementById('stat-avg-price').textContent = '$' + Math.round(avg).toLocaleString();
    } else {
        document.getElementById('stat-avg-price').textContent = 'N/A';
    }

    const modelCounts = {};
    cars.forEach(c => {
        if (c.model) modelCounts[c.model] = (modelCounts[c.model] || 0) + 1;
    });
    const top5 = Object.entries(modelCounts)
        .sort((a, b) => b[1] - a[1])
        .slice(0, 5);

    const list = document.getElementById('stat-top-models');
    if (top5.length === 0) {
        list.innerHTML = '<li class="top-model-item" style="color:#999;">No data yet</li>';
    } else {
        list.innerHTML = top5.map(([model, count]) =>
            `<li class="top-model-item">
                <span class="top-model-name">${escapeHtml(model)}</span>
                <span class="top-model-count">${count}</span>
            </li>`
        ).join('');
    }
}

function populateFilterOptions(cars) {
    const makes = [...new Set(cars.map(c => c.make).filter(Boolean))].sort();
    const models = [...new Set(cars.map(c => c.model).filter(Boolean))].sort();
    const colors = [...new Set(cars.map(c => c.color).filter(Boolean))].sort();

    const makeSelect = document.getElementById('filter-make');
    const curMake = makeSelect.value;
    makeSelect.innerHTML = '<option value="">All Makes</option>';
    makes.forEach(m => { const o = document.createElement('option'); o.value = m; o.textContent = m; makeSelect.appendChild(o); });
    makeSelect.value = curMake;

    const modelSelect = document.getElementById('filter-model');
    const curModel = modelSelect.value;
    modelSelect.innerHTML = '<option value="">All Models</option>';
    models.forEach(m => { const o = document.createElement('option'); o.value = m; o.textContent = m; modelSelect.appendChild(o); });
    modelSelect.value = curModel;

    const colorSelect = document.getElementById('filter-color');
    const curColor = colorSelect.value;
    colorSelect.innerHTML = '<option value="">All Colors</option>';
    colors.forEach(c => { const o = document.createElement('option'); o.value = c; o.textContent = c; colorSelect.appendChild(o); });
    colorSelect.value = curColor;
}

function applyFiltersAndSort() {
    let filtered = [...allCars];

    const makeFilter = document.getElementById('filter-make').value;
    const modelFilter = document.getElementById('filter-model').value;
    const colorFilter = document.getElementById('filter-color').value;

    if (makeFilter) filtered = filtered.filter(c => c.make === makeFilter);
    if (modelFilter) filtered = filtered.filter(c => c.model === modelFilter);
    if (colorFilter) filtered = filtered.filter(c => c.color === colorFilter);

    const sortBy = document.getElementById('sort-by').value;
    switch (sortBy) {
        case 'price-asc': filtered.sort((a, b) => a.price - b.price); break;
        case 'price-desc': filtered.sort((a, b) => b.price - a.price); break;
        case 'year-asc': filtered.sort((a, b) => a.year - b.year); break;
        case 'year-desc': filtered.sort((a, b) => b.year - a.year); break;
        case 'mileage-asc': filtered.sort((a, b) => a.mileageKm - b.mileageKm); break;
        case 'mileage-desc': filtered.sort((a, b) => b.mileageKm - a.mileageKm); break;
    }

    displayCars(filtered);
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
    const countLabel = document.getElementById('cars-count');

    if (!cars || cars.length === 0) {
        container.innerHTML = '<p class="no-cars">No cars match your filters.</p>';
        countLabel.textContent = '';
        return;
    }

    const displayed = cars.slice(0, DISPLAY_LIMIT);
    countLabel.textContent = `Showing ${displayed.length} of ${cars.length}`;

    container.innerHTML = displayed.map(car => `
        <div class="car-card" onclick="showCarPopup(${car.id})">
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
                <button class="btn btn-delete" onclick="deleteCar(${car.id})">Delete</button>
            </div>
        </div>
    `).join('');
}

function showCarPopup(id) {
    const car = allCars.find(c => c.id === id);
    if (!car) return;

    const popup = document.getElementById('car-popup');
    const body = document.getElementById('popup-body');

    const createdDate = car.createdAt ? new Date(car.createdAt).toLocaleDateString() : 'N/A';
    const updatedDate = car.updatedAt ? new Date(car.updatedAt).toLocaleDateString() : 'N/A';

    body.innerHTML = `
        <div class="modal-header">
            <h2>${escapeHtml(car.make)} ${escapeHtml(car.model)}</h2>
            <span class="modal-year">${car.year}</span>
        </div>
        ${car.imageDataUrl
            ? `<img src="${car.imageDataUrl}" alt="${escapeHtml(car.make)}" class="modal-image">`
            : '<div style="text-align:center;padding:40px;background:#f8f9fa;border-radius:8px;margin-bottom:20px;color:#999;">No image available</div>'
        }
        <div class="modal-details">
            <div class="detail-item"><strong>Price</strong><span>$${Number(car.price).toLocaleString()}</span></div>
            <div class="detail-item"><strong>Mileage</strong><span>${Number(car.mileageKm).toLocaleString()} km</span></div>
            ${car.color ? `<div class="detail-item"><strong>Color</strong><span>${escapeHtml(car.color)}</span></div>` : ''}
            ${car.vin ? `<div class="detail-item"><strong>VIN</strong><span>${escapeHtml(car.vin)}</span></div>` : ''}
            <div class="detail-item"><strong>Listed</strong><span>${createdDate}</span></div>
            <div class="detail-item"><strong>Updated</strong><span>${updatedDate}</span></div>
        </div>
        <div class="modal-actions">
            <button class="btn btn-edit" onclick="window.location.href='add-car.html?id=${car.id}'">Edit Listing</button>
            <button class="btn btn-delete" onclick="deleteCarFromPopup(${car.id})">Delete Listing</button>
        </div>
    `;
    popup.style.display = 'block';
}

async function deleteCarFromPopup(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');
        document.getElementById('car-popup').style.display = 'none';
        await loadCars();
    } catch (error) {
        showError('Error deleting car: ' + error.message);
    }
}

async function deleteCar(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');
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

function hideMessage() {
    document.getElementById('error-message').style.display = 'none';
}
