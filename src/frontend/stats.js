const API_URL = 'http://localhost:8080/api/cars';
const STATS_URL = 'http://localhost:8080/api/stats';
let allCars = [];
const DISPLAY_LIMIT = 10;
const FILTER_DEBOUNCE_MS = 150;

function escapeHtml(text) {
    const map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;' };
    return text ? text.replace(/[&<>"']/g, m => map[m]) : '';
}

function debounce(fn, ms) {
    let timeoutId;
    return function (...args) {
        clearTimeout(timeoutId);
        timeoutId = setTimeout(() => fn.apply(this, args), ms);
    };
}

document.addEventListener('DOMContentLoaded', function () {
    loadData();

    document.getElementById('refresh-btn').addEventListener('click', loadData);
    const debouncedApply = debounce(applyFiltersAndSort, FILTER_DEBOUNCE_MS);
    document.getElementById('filter-make').addEventListener('change', debouncedApply);
    document.getElementById('filter-model').addEventListener('change', debouncedApply);
    document.getElementById('filter-color').addEventListener('change', debouncedApply);
    document.getElementById('sort-by').addEventListener('change', debouncedApply);
    document.getElementById('clear-filters').addEventListener('click', clearFilters);

    const popup = document.getElementById('car-popup');
    document.getElementById('popup-close').onclick = () => popup.style.display = 'none';
    window.onclick = e => { if (e.target === popup) popup.style.display = 'none'; };
    document.addEventListener('keydown', e => { if (e.key === 'Escape') popup.style.display = 'none'; });
});

async function loadData() {
    showLoading(true);
    hideMessage();
    try {
        const [statsRes, carsRes] = await Promise.all([
            fetch(STATS_URL),
            fetch(API_URL)
        ]);
        if (!statsRes.ok) throw new Error('Failed to load stats');
        if (!carsRes.ok) throw new Error('Failed to load cars');

        const [stats, cars] = await Promise.all([statsRes.json(), carsRes.json()]);
        allCars = cars;
        renderStats(stats);
        populateFilterOptions(allCars);
        applyFiltersAndSort();
    } catch (error) {
        showError('Error loading data: ' + error.message);
        allCars = [];
        renderStats({ total: 0, avgPrice: 0, topModels: [] });
    } finally {
        showLoading(false);
    }
}

function renderStats(stats) {
    document.getElementById('stat-total').textContent = stats.total ?? 0;

    if (stats.total > 0 && typeof stats.avgPrice === 'number' && !isNaN(stats.avgPrice)) {
        document.getElementById('stat-avg-price').textContent = '$' + Math.round(stats.avgPrice).toLocaleString();
    } else {
        document.getElementById('stat-avg-price').textContent = 'N/A';
    }

    const topModels = stats.topModels || [];
    const list = document.getElementById('stat-top-models');
    if (topModels.length === 0) {
        list.innerHTML = '<li class="top-model-item" style="color:#999;">No data yet</li>';
    } else {
        list.innerHTML = topModels.map(item => {
            const model = item.model || '';
            const count = item.count ?? 0;
            return `<li class="top-model-item">
                <span class="top-model-name">${escapeHtml(model)}</span>
                <span class="top-model-count">${count}</span>
            </li>`;
        }).join('');
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
            </div>
            <div class="car-actions" onclick="event.stopPropagation()">
                <button class="btn btn-edit" onclick="window.location.href='add-car.html?id=${car.id}'">Edit</button>
                <button class="btn btn-delete" onclick="deleteCar(${car.id})">Delete</button>
            </div>
        </div>
    `).join('');
}

async function showCarPopup(id) {
    const popup = document.getElementById('car-popup');
    const body = document.getElementById('popup-body');
    const car = allCars.find(c => c.id === id);
    if (!car) return;

    body.innerHTML = '<div class="loading">Loading details...</div>';
    popup.style.display = 'block';

    try {
        const response = await fetch(`${API_URL}/${id}`);
        if (!response.ok) throw new Error('Failed to load car details');
        const fullCar = await response.json();
        renderPopupContent(body, fullCar);
    } catch (error) {
        body.innerHTML = `<p class="error-message">${escapeHtml(error.message)}</p>`;
    }
}

function renderPopupContent(body, car) {
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
}

async function deleteCarFromPopup(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    const popup = document.getElementById('car-popup');
    popup.style.display = 'none';
    await performDelete(id);
}

async function deleteCar(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    await performDelete(id);
}

async function performDelete(id) {
    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');

        const idx = allCars.findIndex(c => c.id === id);
        if (idx >= 0) {
            allCars.splice(idx, 1);
            const stats = await fetchStats();
            if (stats) renderStats(stats);
            populateFilterOptions(allCars);
            applyFiltersAndSort();
        } else {
            await loadData();
        }
    } catch (error) {
        showError('Error deleting car: ' + error.message);
        await loadData();
    }
}

async function fetchStats() {
    try {
        const res = await fetch(STATS_URL);
        if (!res.ok) return null;
        return await res.json();
    } catch {
        return null;
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
