let ws = new WebSocket('ws://' + window.location.hostname + ':81/');
ws.onopen = function() {
    document.getElementById('connection-status').style.display = 'none';
};
ws.onclose = function() {
    document.getElementById('connection-status').style.display = 'block';
};
ws.onmessage = function(event) {
    let data = event.data.split(',');
    document.getElementById('temp1').innerText = parseFloat(data[0].split(':')[1]).toFixed(1);
    document.getElementById('temp2').innerText = parseFloat(data[1].split(':')[1]).toFixed(1);
    document.getElementById('waterLevel').innerText = parseFloat(data[2].split(':')[1]).toFixed(1);
    document.getElementById('waterTemp').innerText = parseFloat(data[3].split(':')[1]).toFixed(1);
    document.getElementById('batteryLevel').innerText = parseFloat(data[4].split(':')[1]).toFixed(1);
};