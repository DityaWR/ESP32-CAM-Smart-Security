// MQTT Configuration
const mqttConfig = {
    host: "a21d026aee9c471395ac1ab161eb8f48.s1.eu.hivemq.cloud",
    port: 8884, // WebSocket over TLS port
    path: "/mqtt", // WebSocket path
    username: "wololo",
    password: "Wololou6",
    clientId: "WebClient_" + Math.random().toString(16).substr(2, 8),
    useSSL: true
};

// Topics
const topics = {
    status: "perta/status",
    motion: "perta/motion",
    notify: "perta/notify"
};

let client;
let notifications = [];

// Update debug info
function updateDebug(message) {
    const debugEl = document.getElementById("debugInfo");
    if (!debugEl) return; // Safety check
    
    const time = new Date().toLocaleTimeString();
    debugEl.innerHTML = `[${time}] ${message}<br>` + debugEl.innerHTML;
    
    // Keep only last 5 messages
    const lines = debugEl.innerHTML.split('<br>');
    if (lines.length > 5) {
        debugEl.innerHTML = lines.slice(0, 5).join('<br>');
    }
}

// Initialize MQTT client
function initMQTT() {
    console.log("Initializing MQTT connection...");
    updateDebug("Initializing connection to HiveMQ...");
    
    // Create client - HiveMQ format (host, port, clientId)
    client = new Paho.MQTT.Client(
        mqttConfig.host,
        mqttConfig.port,
        mqttConfig.clientId
    );

    // Set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    // Connect options
    const connectOptions = {
        useSSL: mqttConfig.useSSL,
        userName: mqttConfig.username,
        password: mqttConfig.password,
        onSuccess: onConnect,
        onFailure: onConnectFailure,
        reconnect: true,
        keepAliveInterval: 30,
        timeout: 10,
        cleanSession: true,
        mqttVersion: 4
    };

    console.log("Connecting to:", mqttConfig.host + ":" + mqttConfig.port);
    updateDebug(`Connecting to ${mqttConfig.host}:${mqttConfig.port}`);
    
    // Connect to MQTT broker
    try {
        client.connect(connectOptions);
    } catch(error) {
        console.error("Connection error:", error);
        updateDebug("❌ Error: " + error.message);
        document.getElementById("connectionStatus").className = "connection-status disconnected";
        document.getElementById("connectionStatus").innerHTML = "❌ Connection error: " + error.message;
    }
}

// Connection successful
function onConnect() {
    console.log("Connected to MQTT broker");
    updateDebug("✅ Connected successfully!");
    document.getElementById("connectionStatus").className = "connection-status connected";
    document.getElementById("connectionStatus").innerHTML = "✅ Connected to MQTT";

    // Subscribe to topics
    Object.values(topics).forEach(topic => {
        client.subscribe(topic);
        console.log("Subscribed to: " + topic);
        updateDebug("Subscribed to: " + topic);
    });
}

// Connection failed
function onConnectFailure(error) {
    console.error("Connection failed:", error);
    console.error("Error code:", error.errorCode);
    console.error("Error message:", error.errorMessage);
    
    updateDebug(`❌ Failed: Code ${error.errorCode} - ${error.errorMessage}`);
    
    document.getElementById("connectionStatus").className = "connection-status disconnected";
    
    let errorMsg = "❌ Connection failed";
    if (error.errorCode === 7) {
        errorMsg += " - Bad username or password";
    } else if (error.errorCode === 8) {
        errorMsg += " - Not authorized";
    } else {
        errorMsg += " - " + error.errorMessage;
    }
    
    document.getElementById("connectionStatus").innerHTML = errorMsg + ". Retrying...";
    
    // Retry connection after 5 seconds
    setTimeout(() => {
        console.log("Retrying connection...");
        updateDebug("Retrying connection...");
        initMQTT();
    }, 5000);
}

// Connection lost
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.error("Connection lost:", responseObject.errorMessage);
        updateDebug("❌ Connection lost: " + responseObject.errorMessage);
        document.getElementById("connectionStatus").className = "connection-status disconnected";
        document.getElementById("connectionStatus").innerHTML = "❌ Connection lost. Reconnecting...";
    }
}

// Message received
function onMessageArrived(message) {
    const topic = message.destinationName;
    console.log("Message received on topic:", topic);
    console.log("Raw payload:", message.payloadString);
    
    updateDebug(`📨 Message on ${topic}`);
    
    try {
        const payload = JSON.parse(message.payloadString);
        console.log("Parsed payload:", payload);

        switch(topic) {
            case topics.status:
                updateRoomStatus(payload);
                break;
            case topics.motion:
                updateMotionStatus(payload);
                break;
            case topics.notify:
                addNotification(payload);
                break;
            default:
                console.log("Unknown topic:", topic);
        }
    } catch(error) {
        console.error("Error parsing message:", error);
        console.error("Message content:", message.payloadString);
        updateDebug("❌ Parse error: " + error.message);
    }
}

// Update room status
function updateRoomStatus(data) {
    const statusEl = document.getElementById("roomStatus");
    const indicatorEl = document.getElementById("statusIndicator");
    const lastUpdateEl = document.getElementById("lastUpdate");

    if (data.status === "occupied") {
        statusEl.textContent = "RUANGAN SEDANG DIPAKAI";
        indicatorEl.className = "status-indicator status-occupied";
    } else {
        statusEl.textContent = "RUANGAN TIDAK DIPAKAI";
        indicatorEl.className = "status-indicator status-empty";
    }

    lastUpdateEl.textContent = "Last update: " + new Date().toLocaleTimeString('id-ID');
}

// Update motion status
function updateMotionStatus(data) {
    const iconEl = document.getElementById("motionIcon");
    const textEl = document.getElementById("motionText");

    if (data.motion) {
        iconEl.className = "motion-icon motion-detected";
        iconEl.textContent = "🚨";
        textEl.textContent = "Gerakan terdeteksi!";
    } else {
        iconEl.className = "motion-icon motion-none";
        iconEl.textContent = "👤";
        textEl.textContent = "Tidak ada gerakan";
    }
}

// Add notification
function addNotification(data) {
    const notificationList = document.getElementById("notificationList");
    const time = new Date().toLocaleString('id-ID');
    
    const notificationEl = document.createElement("div");
    notificationEl.className = "notification-item";
    notificationEl.innerHTML = `
        <div class="notification-time">${time}</div>
        <div class="notification-message">${data.message}</div>
    `;

    // Add to beginning of list
    notificationList.insertBefore(notificationEl, notificationList.firstChild);

    // Keep only last 10 notifications
    while (notificationList.children.length > 10) {
        notificationList.removeChild(notificationList.lastChild);
    }
}

// Test connection function
function testConnection() {
    console.log("Testing MQTT connection...");
    console.log("Host:", mqttConfig.host);
    console.log("Port:", mqttConfig.port);
    console.log("Path:", mqttConfig.path);
    console.log("Client ID:", mqttConfig.clientId);
    console.log("Connected:", client && client.isConnected());
    
    if (client && client.isConnected()) {
        alert("Connected to MQTT broker!");
        // Send test message
        const testMessage = new Paho.MQTT.Message(JSON.stringify({test: true, time: new Date().toISOString()}));
        testMessage.destinationName = "perta/test";
        client.send(testMessage);
    } else {
        alert("Not connected to MQTT broker. Check console for details.");
    }
}

// Reconnect function
function reconnect() {
    console.log("Manual reconnect triggered");
    if (client && client.isConnected()) {
        client.disconnect();
    }
    setTimeout(() => {
        initMQTT();
    }, 1000);
}

// Alternative connection method for HiveMQ
function initMQTTAlternative() {
    console.log("Trying alternative connection method...");
    updateDebug("Trying alternative method...");
    
    // Try with explicit WebSocket URL and path
    client = new Paho.MQTT.Client(
        mqttConfig.host,
        mqttConfig.port,
        "/mqtt",  // Add path as third parameter
        mqttConfig.clientId
    );
    
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    
    const connectOptions = {
        useSSL: mqttConfig.useSSL,
        userName: mqttConfig.username,
        password: mqttConfig.password,
        onSuccess: onConnect,
        onFailure: (error) => {
            console.error("Alternative connection also failed:", error);
            updateDebug("❌ Alt failed: " + error.errorMessage);
            onConnectFailure(error);
        },
        timeout: 10,
        keepAliveInterval: 30,
        cleanSession: true,
        mqttVersion: 4
    };
    
    try {
        client.connect(connectOptions);
    } catch(error) {
        console.error("Alternative connection error:", error);
        updateDebug("❌ Alt error: " + error.message);
    }
}

// Initialize when page loads
window.onload = function() {
    console.log("=== MQTT Debug Info ===");
    console.log("Page loaded, initializing MQTT...");
    
    // Check if Paho library is loaded
    if (typeof Paho === 'undefined') {
        console.error("❌ Paho MQTT library not loaded!");
        updateDebug("❌ MQTT library not loaded");
        document.getElementById("connectionStatus").innerHTML = "❌ MQTT library failed to load";
        return;
    }
    
    console.log("✅ Paho MQTT library loaded successfully");
    console.log("Library version:", Paho.MQTT.VERSION || "Unknown");
    
    // Show configuration
    console.log("Configuration:");
    console.log("- Host:", mqttConfig.host);
    console.log("- Port:", mqttConfig.port);
    console.log("- SSL:", mqttConfig.useSSL);
    console.log("- Client ID:", mqttConfig.clientId);
    console.log("===================");
    
    // Try primary connection method
    initMQTT();
    
    // If not connected after 10 seconds, try alternative
    setTimeout(() => {
        if (!client || !client.isConnected()) {
            console.log("Primary connection failed after 10s, trying alternative...");
            updateDebug("Trying alternative connection...");
            initMQTTAlternative();
        }
    }, 10000);
};

console.log("MQTT Dashboard loaded. Open browser console (F12) for debugging info.");