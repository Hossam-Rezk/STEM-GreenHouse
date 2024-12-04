// Set up the temperature chart
const tempCtx = document.getElementById('temperatureChart').getContext('2d');
const tempChart = new Chart(tempCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Temperature (°C)',
            data: [],
            borderColor: 'rgba(255, 99, 132, 1)',
            backgroundColor: 'rgba(255, 99, 132, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { title: { display: true, text: 'Temperature (°C)' } }
        }
    }
});

// Set up the humidity chart
const humidityCtx = document.getElementById('humidityChart').getContext('2d');
const humidityChart = new Chart(humidityCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Humidity (%)',
            data: [],
            borderColor: 'rgba(75, 192, 192, 1)',
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { title: { display: true, text: 'Humidity (%)' } }
        }
    }
});

// Set up the soil moisture chart
const moistureCtx = document.getElementById('moistureChart').getContext('2d');
const moistureChart = new Chart(moistureCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Soil Moisture (%)',
            data: [],
            borderColor: 'rgba(153, 102, 255, 1)',
            backgroundColor: 'rgba(153, 102, 255, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { title: { display: true, text: 'Soil Moisture (%)' } }
        }
    }
});

// Function to fetch data from the ESP32 server
async function fetchSensorData() {
    try {
        const response = await fetch('http://192.168.1.7/data'); // Replace with your ESP32 IP
        const data = await response.json();

        const now = new Date().toLocaleTimeString();

        // Update Temperature Chart
        tempChart.data.labels.push(now);
        tempChart.data.datasets[0].data.push(data.temperature);
        tempChart.update();

        // Update Humidity Chart
        humidityChart.data.labels.push(now);
        humidityChart.data.datasets[0].data.push(data.humidity);
        humidityChart.update();

        // Update Soil Moisture Chart
        moistureChart.data.labels.push(now);
        moistureChart.data.datasets[0].data.push(data.soilMoisture);
        moistureChart.update();

        // Update Stats
        document.getElementById('avg-temp').textContent = `${data.temperature} °C`;
        document.getElementById('avg-humidity').textContent = `${data.humidity} %`;
        document.getElementById('avg-moisture').textContent = `${data.soilMoisture} %`;

        // Motion Detected Box
        const motionBox = document.getElementById('motionStatus');
        motionBox.textContent = data.motionDetected ? "Motion Detected" : "No Motion Detected";
        motionBox.style.backgroundColor = data.motionDetected ? "red" : "green"; // Red for motion, green for no motion

        // Check for Thresholds
        const thresholdWarning = document.getElementById('warning');
        if (data.thresholdExceeded) {
            thresholdWarning.style.display = 'block';
            thresholdWarning.textContent = `Warning: Threshold Exceeded!`;
        } else {
            thresholdWarning.style.display = 'none';
        }

    } catch (error) {
        console.error("Error fetching data from ESP:", error);
    }
}

// Set an interval to fetch data every 2 seconds
setInterval(fetchSensorData, 2000);
