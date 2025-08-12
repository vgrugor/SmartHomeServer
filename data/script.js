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
    fetch("/", { method: 'HEAD', cache: "no-store" })
        .then(response => {
            if (!response.ok) throw new Error();
            console.log("ESP online");
        })
        .catch(err => {
            document.getElementById("connection-status").style.display = "block";
            document.getElementById("connection-status").innerText =
                "⚠️ Контролер недоступний. Сторінка можливо збережена браузером офлайн.";
        });
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

function updateSliderPWM(element) {
    var sliderNumber = element.id.charAt(element.id.length - 1);
    var slider = document.getElementById(element.id);
    var newValue = slider.value;
    var valueDisplay = document.getElementById("sliderValue" + sliderNumber);
    var oldValue = valueDisplay.innerHTML;

    valueDisplay.innerHTML = newValue;

    if (isConnected) {
        websocket.send(sliderNumber + "s" + newValue.toString());
    } else {
        alert("⚠️ Контролер недоступний. Зміни не збережено.");

        slider.value = oldValue;
        valueDisplay.innerHTML = oldValue;
    }
}

function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++) {
        var key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
        var slider = document.getElementById("slider" + (i + 1));
        if (slider) {
            slider.value = myObj[key];
        }
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
