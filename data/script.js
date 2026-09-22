// Complete project details: https://randomnerdtutorials.com/esp8266-nodemcu-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var isConnected = false;

window.addEventListener('load', () => {
    highlightActiveTab();
    checkOnlineStatus();
    initWebSocket();
});

function checkOnlineStatus() {
    if (isConnected != true) {
        fetch("/", { method: 'HEAD', cache: "no-store" })
            .then(response => {
                if (!response.ok) throw new Error();
                console.log("ESP online");
            })
            .catch(err => {
                document.getElementById("connection-status").style.display = "block";
                document.getElementById("connection-status").innerText =
                    "⚠️ Контролер недоступний. Сторінка можливо збережена браузером офлайн.";
            }
        );
    }
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    isConnected = true;
    document.getElementById("connection-status").style.display = "none";
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    isConnected = false;
    document.getElementById("connection-status").style.display = "block";
    setTimeout(initWebSocket, 2000);
}

function getValues() {
    websocket.send("getValues");
}

function onMessage(event) {
    console.log(event.data);

    try {
        const values = JSON.parse(event.data);

        Object.entries(values).forEach(([elementId, value]) => {
            const element = document.getElementById(elementId);
            if (element) {
                element.textContent = value;
            }
        });
    } catch (error) {
        console.error("Invalid WebSocket payload", error);
    }
}

function highlightActiveTab() {
    const currentHost = window.location.hostname;
    const tabs = document.querySelectorAll('.tab-container .tab');

    tabs.forEach(tab => {
        if (tab.dataset.ip === currentHost) {
            tab.classList.add('active');
        } else {
            tab.classList.remove('active');
        }
    });
}
