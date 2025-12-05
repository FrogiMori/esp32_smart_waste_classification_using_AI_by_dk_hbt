// API Configuration
const API_BASE = 'http://localhost:5000';
let timeChart = null;

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    initializeCharts();
    loadStatistics();
    loadHistory();
    
    // Auto refresh every 30 seconds
    setInterval(() => {
        loadStatistics();
        loadHistory();
    }, 30000);
});

// Statistics
async function loadStatistics() {
    try {
        const response = await fetch(`${API_BASE}/api/statistics`);
        const data = await response.json();

        // Update counts
        document.getElementById('recycleCount').textContent = data.recycle || 0;
        document.getElementById('organicCount').textContent = data.organic || 0;
        document.getElementById('otherCount').textContent = data.other || 0;
        document.getElementById('totalCount').textContent = data.total || 0;

        // Update percentages
        const total = data.total || 1;
        document.getElementById('recyclePercent').textContent = 
            total > 0 ? ((data.recycle / total) * 100).toFixed(1) + '%' : '0%';
        document.getElementById('organicPercent').textContent = 
            total > 0 ? ((data.organic / total) * 100).toFixed(1) + '%' : '0%';
        document.getElementById('otherPercent').textContent = 
            total > 0 ? ((data.other / total) * 100).toFixed(1) + '%' : '0%';

        // Update chart
        updateTimeChart(data.timeSeries || []);
    } catch (error) {
        console.error('Error loading statistics:', error);
    }
}

// Charts
function initializeCharts() {
    // Time Chart
    const timeCtx = document.getElementById('timeChart').getContext('2d');
    timeChart = new Chart(timeCtx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'RECYCLE',
                    data: [],
                    borderColor: '#3498db',
                    backgroundColor: 'rgba(52, 152, 219, 0.1)',
                    tension: 0.4,
                    fill: true
                },
                {
                    label: 'ORGANIC',
                    data: [],
                    borderColor: '#27ae60',
                    backgroundColor: 'rgba(39, 174, 96, 0.1)',
                    tension: 0.4,
                    fill: true
                },
                {
                    label: 'OTHER',
                    data: [],
                    borderColor: '#e74c3c',
                    backgroundColor: 'rgba(231, 76, 60, 0.1)',
                    tension: 0.4,
                    fill: true
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: true,
            plugins: {
                legend: {
                    position: 'top',
                    labels: {
                        padding: 15,
                        font: {
                            size: 12,
                            weight: 'bold'
                        }
                    }
                },
                tooltip: {
                    mode: 'index',
                    intersect: false
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    ticks: {
                        stepSize: 1
                    }
                }
            }
        }
    });
}

function updateTimeChart(timeSeries) {
    if (!timeSeries || timeSeries.length === 0) {
        // Generate sample data for last 7 days if no data
        const labels = [];
        const recycleData = [];
        const organicData = [];
        const otherData = [];

        for (let i = 6; i >= 0; i--) {
            const date = new Date();
            date.setDate(date.getDate() - i);
            labels.push(date.toLocaleDateString('vi-VN', { day: '2-digit', month: '2-digit' }));
            recycleData.push(Math.floor(Math.random() * 10));
            organicData.push(Math.floor(Math.random() * 10));
            otherData.push(Math.floor(Math.random() * 5));
        }

        timeChart.data.labels = labels;
        timeChart.data.datasets[0].data = recycleData;
        timeChart.data.datasets[1].data = organicData;
        timeChart.data.datasets[2].data = otherData;
    } else {
        const labels = timeSeries.map(item => {
            const date = new Date(item.date);
            return date.toLocaleDateString('vi-VN', { day: '2-digit', month: '2-digit' });
        });
        const recycleData = timeSeries.map(item => item.recycle || 0);
        const organicData = timeSeries.map(item => item.organic || 0);
        const otherData = timeSeries.map(item => item.other || 0);

        timeChart.data.labels = labels;
        timeChart.data.datasets[0].data = recycleData;
        timeChart.data.datasets[1].data = organicData;
        timeChart.data.datasets[2].data = otherData;
    }
    timeChart.update();
}

// History
async function loadHistory() {
    try {
        const response = await fetch(`${API_BASE}/api/history`);
        const history = await response.json();
        displayHistory(history);
    } catch (error) {
        console.error('Error loading history:', error);
        document.getElementById('historyList').innerHTML = 
            '<div class="history-empty">Không thể tải lịch sử</div>';
    }
}

function displayHistory(history) {
    const historyList = document.getElementById('historyList');
    
    if (!history || history.length === 0) {
        historyList.innerHTML = '<div class="history-empty">Chưa có lịch sử phân loại</div>';
        return;
    }

    historyList.innerHTML = history.map(item => `
        <div class="history-item">
            <img src="${item.image_url || 'data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAiIGhlaWdodD0iODAiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PHJlY3Qgd2lkdGg9IjgwIiBoZWlnaHQ9IjgwIiBmaWxsPSIjZGRkIi8+PHRleHQgeD0iNTAlIiB5PSI1MCUiIGZvbnQtc2l6ZT0iMTIiIGZpbGw9IiM5OTkiIHRleHQtYW5jaG9yPSJtaWRkbGUiIGR5PSIuM2VtIj5ObyBJbWFnZTwvdGV4dD48L3N2Zz4='}" 
                 alt="History" class="history-image" 
                 onerror="this.src='data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAiIGhlaWdodD0iODAiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PHJlY3Qgd2lkdGg9IjgwIiBoZWlnaHQ9IjgwIiBmaWxsPSIjZGRkIi8+PHRleHQgeD0iNTAlIiB5PSI1MCUiIGZvbnQtc2l6ZT0iMTIiIGZpbGw9IiM5OTkiIHRleHQtYW5jaG9yPSJtaWRkbGUiIGR5PSIuM2VtIj5ObyBJbWFnZTwvdGV4dD48L3N2Zz4='">
            <div class="history-info">
                <div class="history-label ${item.label.toLowerCase()}">${item.label}</div>
                <div class="history-time">${formatTime(item.timestamp)}</div>
            </div>
        </div>
    `).join('');
}

function formatTime(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diff = now - date;
    const minutes = Math.floor(diff / 60000);
    const hours = Math.floor(diff / 3600000);
    const days = Math.floor(diff / 86400000);

    if (minutes < 1) return 'Vừa xong';
    if (minutes < 60) return `${minutes} phút trước`;
    if (hours < 24) return `${hours} giờ trước`;
    if (days < 7) return `${days} ngày trước`;
    
    return date.toLocaleDateString('vi-VN', {
        day: '2-digit',
        month: '2-digit',
        year: 'numeric',
        hour: '2-digit',
        minute: '2-digit'
    });
}

// Refresh button
document.getElementById('refreshBtn').addEventListener('click', () => {
    loadStatistics();
    loadHistory();
});

