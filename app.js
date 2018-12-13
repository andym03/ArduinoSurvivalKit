var myBtn = document.getElementById('connectBtn');
var byeBtn = document.getElementById('disconnectBtn');
var myPre = document.getElementById('info');

var jacketJourney = [];
var addLat = true;
var addData = false;
var map;
var poly

myBtn.addEventListener('click', doConnect);
//byeBtn.addEventListener('click', doDisconnect);

function initMap() {
    map = new google.maps.Map(document.getElementById('map'), {
        zoom: 3,
        center: {lat: 0, lng: -180},
        mapTypeId: 'terrain'
    });

    poly = new google.maps.Polyline({
        map: map,
        path: [],
        geodesic: true,
        strokeColor: '#FF0000',
        strokeOpacity: 1.0,
        strokeWeight: 2
  })
}

function drawOnMap() {

    var path = poly.getPath();

    for (i in jacketJourney) {
        path.push(new google.maps.LatLng(Number(i.lat, Number(i.lng))));
        console.log({i});
    }
    poly.setPath(path);
   
    jacketJourney = [];
}

function doConnect() {
    // Obtain configured instance.
    let terminal = new BluetoothTerminal();

    // Request the device for connection and get its name after successful connection.
    terminal.connect().then(() => {
        startTime = new Date().getTime();
        // alert(terminal.getDeviceName() + ' is connected!');
    });

    var gpsPos = {lat: 0.0, lng: 0.0};
    // Override `receive` method to handle incoming data as you want.
    terminal.receive = (data) => {
        if (data == "FINISHED GPS") {
            addData = false;
            //drawOnMap();
            drawOnMap();
            terminal.disconnect();
        }
        if (addData == true) {
            if (addLat == true) {
                gpsPos.lat = parseFloat(data);
                addLat = false;

                console.log("Latitude: " + data);
            } else {
                gpsPos.lng = parseFloat(data);
                addLat = true;
                console.log("Longitude: " + data);

                jacketJourney.push({ ...gpsPos });
                gpsPos = {lat: 0.0, lng: 0.0};
            }
        }
        if (data == "SENDING GPS") {
            addData = true;
        }
    }

    // Send something to the connected device.
    // terminal.send('Simon says: Hello, world!');

    // Disconnect from the connected device.
    terminal.disconnect();
}
