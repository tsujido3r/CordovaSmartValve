// Server address and pin numbers of the board (ESP8266/32 and compatibles)
let addr = "";
let smartValveAddr = "";
let ipConnected = false;

//Pin mapping for ESP32 - modify for ESP8266
let pins = new Map([
    ['#valve1', '12'],
    ['#valve2', '13'],
    ['#valve3', '32'],
    ['#valve4', '33'],
    ['#valve5', '25'],
    ['#valve6', '26'],
    ['#valve7', '27'],
    ['#valve8', '14'],
    ['#valve9', '16'],
]);

// Click Event on switch Class - Sends pins status to ESP on button press  
$('.sw').on('click', function (e) {
    if (ipConnected == true) {
        let onoff = ['OFF', 'ON'];
        let id = "#" + $(this).attr("id"); // Get the id of the control
        let pin = pins.get(id); // Pin number
        let value = String(+$(id).prop('checked')); // On or Off
        $(id + 'v').html(onoff[value]);

        sendAjax(addr, pin, value); // Sends POST command to the server with pin number and value
    } else {
        //Wait till licent client is connected to server before sendind any commands
        ons.notification.alert('Please enter IP and connect before operations!');
    }
});

// Input event on range class - PWM pin
$('.rg').on('input', function (e) {
    if (ipConnected == true) {
        let id = "#" + $(this).attr("id"); // Get the id of the control
        let value = String($(id).val()); // Input range
        $(id + 'v').html(value); // Notification
    } else {
        //Wait till licent client is connected to server before sendind any commands
        ons.notification.alert('Please enter IP and connect before operations!');
    }
});

$('.rg').on('change', function (e) {
    if (ipConnected == true) {
        let id = "#" + $(this).attr("id"); // Get the id of the control
        let pin = pins.get(id); // Pin number
        let value = String($(id).val()); // Input range
        sendAjax(addr, pin, value);
    } else {
        //Wait till licent client is connected to server before sendind any commands
        ons.notification.alert('Please enter IP and connect before operations!');
    }
});


function sendAjax(addr, p, v) {
    $.ajax({
        method: "POST",
        url: addr,
        data: { pin: p, value: v }
    });
}


const connect = () => {
    smartValveAddr = document.querySelector('#serverIP').value;

    var ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
    if (smartValveAddr.match(ipformat)) {
        document.getElementById("serverIP").remove();
        document.getElementById("connectButton").remove();
        document.getElementById("statusText").innerHTML = "Connecting to server Smart Valve Server " + smartValveAddr + "...";
        ipConnected = true;
        addr = "http://" + smartValveAddr + "/command";
        setTimeout(function () {
            document.getElementById("statusText").innerHTML = "Connected to server Smart Valve Server @" + smartValveAddr;
        }, 1500);
    }
    else {
        ons.notification.alert('Wrong IP format! Please reenter the correct IP address.');
        ipConnected = true;
    }
}
