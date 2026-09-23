// Complete project details: https://randomnerdtutorials.com/esp8266-nodemcu-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var isConnected = false;
var updateAges = {};

window.addEventListener('load', () => {
    highlightActiveTab();
    checkOnlineStatus();
    initWebSocket();
    setInterval(refreshUpdateAges, 60000);
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
            if (elementId.endsWith("AgeMinutes")) {
                rememberUpdateAge(elementId.replace("AgeMinutes", ""), value);
                return;
            }

            const element = document.getElementById(elementId);
            if (element) {
                element.textContent = value;
            }
        });
    } catch (error) {
        console.error("Invalid WebSocket payload", error);
    }
}

function rememberUpdateAge(valueElementId, ageMinutes) {
    updateAges[valueElementId] = {
        ageMinutes: ageMinutes === null ? null : Number(ageMinutes),
        receivedAtMs: Date.now()
    };
    renderUpdateAge(valueElementId);
}

function refreshUpdateAges() {
    Object.keys(updateAges).forEach(renderUpdateAge);
}

function renderUpdateAge(valueElementId) {
    const ageElement = document.getElementById(`${valueElementId}Age`);
    const valueElement = document.getElementById(valueElementId);
    const valueContainer = valueElement?.closest(".sensor-value");
    const age = updateAges[valueElementId];

    if (!ageElement || !age) {
        return;
    }

    if (age.ageMinutes === null || !Number.isFinite(age.ageMinutes)) {
        ageElement.textContent = "ще не оновлено";
        valueContainer?.classList.remove("is-stale");
        return;
    }

    const minutesSinceMessage = Math.floor((Date.now() - age.receivedAtMs) / 60000);
    const totalMinutes = Math.max(0, age.ageMinutes + minutesSinceMessage);
    ageElement.textContent = formatUpdateAge(totalMinutes);
    valueContainer?.classList.toggle("is-stale", totalMinutes >= 60);
}

function formatUpdateAge(totalMinutes) {
    const minutes = Math.max(0, Math.floor(totalMinutes));

    if (minutes === 0) {
        return "оновлено: щойно";
    }

    if (minutes < 60) {
        return `оновлено: ${minutes} хв тому`;
    }

    if (minutes < 1440) {
        const hours = Math.floor(minutes / 60);
        const remainingMinutes = minutes % 60;
        const minutePart = remainingMinutes > 0 ? ` ${remainingMinutes} хв` : "";
        return `оновлено: ${hours} год${minutePart} тому`;
    }

    const days = Math.max(1, Math.round(minutes / 1440));
    return `оновлено: ${days} ${ukrainianDayWord(days)} тому`;
}

function ukrainianDayWord(days) {
    const lastTwoDigits = days % 100;
    const lastDigit = days % 10;

    if (lastTwoDigits >= 11 && lastTwoDigits <= 14) {
        return "днів";
    }

    if (lastDigit === 1) {
        return "день";
    }

    if (lastDigit >= 2 && lastDigit <= 4) {
        return "дні";
    }

    return "днів";
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
