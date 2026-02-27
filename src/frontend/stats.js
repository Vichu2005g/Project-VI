const API_URL = 'http://localhost:8080/api/cars';
let allCars = [];
let currentOffset = 0;
const PAGE_SIZE = 20;
let totalCars = 0;

function escapeHtml(text) {
    const map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;' };
    return text ? text.replace(/[&<>"']/g, m => map[m]) : '';
}

document.addEventListener('DOMContentLoaded', function () {
    loadCars(0);

    document.getElementById('refresh-btn').addEventListener('click', () => loadCars(0));
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

async function loadCars(offset = 0) {
    showLoading(true);
    hideMessage();
    try {
        // Using paginated API 
        const response = await fetch(`${API_URL}?limit=${PAGE_SIZE}&offset=${offset}`);
        if (!response.ok) throw new Error('Failed to load cars');
        const data = await response.json();

        // added brand new sweet sweet API returns 
        allCars = data.cars || data; 
        totalCars = data.total || allCars.length;
        currentOffset = offset;

        populateFilterOptions(allCars);
        computeStats(allCars, totalCars);
        applyFiltersAndSort();
        renderPagination();
    } catch (error) {
        showError('Error loading cars: ' + error.message);
        allCars = [];
    } finally {
        showLoading(false);
    }
}

function renderPagination() {
    let paginationEl = document.getElementById('pagination');
    if (!paginationEl) {
        paginationEl = document.createElement('div');
        paginationEl.id = 'pagination';
        paginationEl.style.cssText = 'display:flex;gap:10px;justify-content:center;align-items:center;margin-top:20px;';
        document.getElementById('cars-container').after(paginationEl);
    }

    const totalPages = Math.ceil(totalCars / PAGE_SIZE);
    const currentPage = Math.floor(currentOffset / PAGE_SIZE) + 1;

    if (totalPages <= 1) {
        paginationEl.innerHTML = '';
        return;
    }

    paginationEl.innerHTML = `
        <button onclick="loadCars(${currentOffset - PAGE_SIZE})"
            style="padding:8px 16px;border-radius:5px;border:none;background:#667eea;color:white;cursor:pointer;"
            ${currentOffset === 0 ? 'disabled style="background:#ccc;cursor:default;"' : ''}>
            ← Prev
        </button>
        <span style="color:#333;font-weight:500;">Page ${currentPage} of ${totalPages} (${totalCars} total)</span>
        <button onclick="loadCars(${currentOffset + PAGE_SIZE})"
            style="padding:8px 16px;border-radius:5px;border:none;background:#667eea;color:white;cursor:pointer;"
            ${currentOffset + PAGE_SIZE >= totalCars ? 'disabled style="background:#ccc;cursor:default;"' : ''}>
            Next →
        </button>
    `;
}

function computeStats(cars, total) {
    // Total uses the API's total count, not just current page
    document.getElementById('stat-total').textContent = total || cars.length;

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
    const top5 = Object.entries(modelCounts).sort((a, b) => b[1] - a[1]).slice(0, 5);

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
    const makes   = [...new Set(cars.map(c => c.make).filter(Boolean))].sort();
    const models  = [...new Set(cars.map(c => c.model).filter(Boolean))].sort();
    const colors  = [...new Set(cars.map(c => c.color).filter(Boolean))].sort();

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

    const makeFilter  = document.getElementById('filter-make').value;
    const modelFilter = document.getElementById('filter-model').value;
    const colorFilter = document.getElementById('filter-color').value;

    if (makeFilter)  filtered = filtered.filter(c => c.make  === makeFilter);
    if (modelFilter) filtered = filtered.filter(c => c.model === modelFilter);
    if (colorFilter) filtered = filtered.filter(c => c.color === colorFilter);

    const sortBy = document.getElementById('sort-by').value;
    switch (sortBy) {
        case 'price-asc':    filtered.sort((a, b) => a.price    - b.price);    break;
        case 'price-desc':   filtered.sort((a, b) => b.price    - a.price);    break;
        case 'year-asc':     filtered.sort((a, b) => a.year     - b.year);     break;
        case 'year-desc':    filtered.sort((a, b) => b.year     - a.year);     break;
        case 'mileage-asc':  filtered.sort((a, b) => a.mileageKm - b.mileageKm); break;
        case 'mileage-desc': filtered.sort((a, b) => b.mileageKm - a.mileageKm); break;
    }

    displayCars(filtered);
}

function clearFilters() {
    document.getElementById('filter-make').value  = '';
    document.getElementById('filter-model').value = '';
    document.getElementById('filter-color').value = '';
    document.getElementById('sort-by').value      = '';
    applyFiltersAndSort();
}

function displayCars(cars) {
    const container  = document.getElementById('cars-container');
    const countLabel = document.getElementById('cars-count');

    if (!cars || cars.length === 0) {
        container.innerHTML = '<p class="no-cars">No cars match your filters.</p>';
        countLabel.textContent = '';
        return;
    }

    countLabel.textContent = `Showing ${cars.length} of ${totalCars}`;

    container.innerHTML = cars.map(car => `
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
                <button class="btn btn-patch" onclick="openQuickEdit(event, ${car.id}, ${car.price}, '${car.color}')">Quick Edit</button>
                <button class="btn btn-delete" onclick="deleteCar(${car.id})">Delete</button>
            </div>
        </div>
    `).join('');
}

// showCarPopup fetches the full car with img only when user clicks 
async function showCarPopup(id) {
    const popup = document.getElementById('car-popup');
    const body  = document.getElementById('popup-body');

    body.innerHTML = '<div style="text-align:center;padding:40px;">Loading...</div>';
    popup.style.display = 'block';

    try {
        // Fetches full car including image only when needed
        const response = await fetch(`${API_URL}/${id}`);
        if (!response.ok) throw new Error('Car not found');
        const car = await response.json();

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
                ${car.vin   ? `<div class="detail-item"><strong>VIN</strong><span>${escapeHtml(car.vin)}</span></div>`   : ''}
                <div class="detail-item"><strong>Listed</strong><span>${createdDate}</span></div>
                <div class="detail-item"><strong>Updated</strong><span>${updatedDate}</span></div>
            </div>
            <div class="modal-actions">
                <button class="btn btn-edit"   onclick="window.location.href='add-car.html?id=${car.id}'">Edit Listing</button>
                <button class="btn btn-delete" onclick="deleteCarFromPopup(${car.id})">Delete Listing</button>
            </div>
        `;
    } catch (error) {
        body.innerHTML = `<p style="color:red;padding:20px;">Error loading car: ${error.message}</p>`;
    }
}

async function deleteCarFromPopup(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');
        document.getElementById('car-popup').style.display = 'none';
        await loadCars(currentOffset);
    } catch (error) {
        showError('Error deleting car: ' + error.message);
    }
}

async function deleteCar(id) {
    if (!confirm('Are you sure you want to delete this car listing?')) return;
    try {
        const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
        if (!response.ok) throw new Error('Failed to delete car');
        await loadCars(currentOffset);
    } catch (error) {
        showError('Error deleting car: ' + error.message);
    }
}

function showLoading(show) { document.getElementById('loading').style.display = show ? 'block' : 'none'; }

function showError(message) {
    const div = document.getElementById('error-message');
    div.textContent = message;
    div.className = 'error-message';
    div.style.display = 'block';
    setTimeout(() => hideMessage(), 5000);
}

// ============================================
// PATCH — Quick Edit (partial update)
// Only sends the fields the user actually changed
// ============================================

function openQuickEdit(event, carId, currentPrice, currentColor) {
    event.stopPropagation(); // prevent car popup from opening

    document.getElementById('patch-car-id').value  = carId;
    document.getElementById('patch-price').value   = currentPrice;
    document.getElementById('patch-color').value   = currentColor;

    updatePatchPreview();

    document.getElementById('quick-edit-modal').style.display = 'block';

    // Live preview updates as user types
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

    if (!price || price < 0) {
        alert('Please enter a valid price');
        return;
    }
    if (!color) {
        alert('Please enter a color');
        return;
    }

    // PATCH only sends price and color — not the whole car object
    // This is the key difference from PUT
    const patchData = {
        price: price,
        color: color
    };

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
        await loadCars(currentOffset); // refresh the list

    } catch (error) {
        alert('Error applying quick edit: ' + error.message);
    }
}

// Close modal when clicking outside
document.getElementById('quick-edit-modal')
    .addEventListener('click', function(e) {
        if (e.target === this) closeQuickEdit();
    });

function hideMessage() { document.getElementById('error-message').style.display = 'none'; }