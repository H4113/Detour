
 
function processData(obj){
	alert("packet entier, taille: "+obj.buffer.byteLength);
	var data = obj.buffer;
	if(obj.type == 1){ // type == 1 -> PATH sent
		var path = parseData(data);
		drawPathOnMap(Map.map, path);
	}
}

function sendQuery(buf) {
	chrome.socket.create('tcp', {}, function(createInfo) {
		//alert(createInfo);
		var socketId = createInfo.socketId;
		function readPackets(readInfo) {
			alert("packet partiel, taille:"+readInfo.data.byteLength);
			magicTcpReceive(readInfo.data, processData);
			chrome.socket.read(socketId, null, readPackets);
		}
		chrome.socket.connect(socketId, "192.168.1.199", 6666, function(result) {
			if(result >= 0) {
				alert("prout " + result);
				chrome.socket.write(socketId, buf, function(writeInfo) {
					if(writeInfo.bytesWritten > 0) {
						alert("ouiiiii ");
						chrome.socket.read(socketId, null, readPackets);
						//chrome.socket.disconnect(socketId);
					} else {
						// Cannot send data
						// Do something smart 
					}
				});
			} else {
				alert("pas de serveur");
				// Cannot connect
				// Do something smart
			}

		});
	});
}

var app = {
	// Application Constructor
	initialize: function() {
		this.bindEvents();
	},
	// Bind Event Listeners
	//
	// Bind any events that are required on startup. Common events are:
	// 'load', 'deviceready', 'offline', and 'online'.
	bindEvents: function() {
		document.addEventListener('deviceready', this.onDeviceReady, false);
	},
	// deviceready Event Handler
	//
	// The scope of 'this' is the event. In order to call the 'receivedEvent'
	// function, we must explicitly call 'app.receivedEvent(...);'
	onDeviceReady: function() {
		app.receivedEvent('deviceready');
		var buf = new ArrayBuffer(8+4*8);
		var type_req = new Int16Array(buf,0,1);
		var type_junk = new Int16Array(buf,2,1);
		var type_junk2 = new Int32Array(buf,4,1);
		var gpscoord = new Float64Array(buf,8,4);
		
		type_req[0] = 0;
		type_junk[0] = 42;
		type_junk2[0] = 42;
	
	var onSuccess = function(position) {
			/*alert('Latitude: '			  + position.coords.latitude		  + '\n' +
					  'Longitude: '			+ position.coords.longitude			+ '\n' +
					  'Altitude: '			+ position.coords.altitude			+ '\n' +
					  'Accuracy: '			+ position.coords.accuracy			+ '\n' +
					  'Altitude Accuracy: ' + position.coords.altitudeAccuracy	+ '\n' +
					  'Heading: '			+ position.coords.heading			+ '\n' +
					  'Speed: '				+ position.coords.speed				+ '\n' +
					  'Timestamp: '			+ position.timestamp				+ '\n');*/
			gpscoord[0] = position.coords.latitude;
			gpscoord[1] = position.coords.longitude; 
			gpscoord[2] = 45.6803042752;
			gpscoord[3] = 4.92207816575;
			sendQuery(buf);
		};
	
		// onError Callback receives a PositionError object
		//
		function onError(error) {
			alert('code: '	+ error.code	+ '\n' +
				'message: ' + error.message + '\n');
		}
	
		navigator.geolocation.getCurrentPosition(onSuccess, onError);
	},
	// Update DOM on a Received Event
	receivedEvent: function(id) {
		/*var parentElement = document.getElementById(id);
		var listeningElement = parentElement.querySelector('.listening');
		var receivedElement = parentElement.querySelector('.received');
		
		listeningElement.setAttribute('style', 'display:none;');
		receivedElement.setAttribute('style', 'display:block;');*/
		
		console.log('Received Event: ' + id);
	}
};

app.initialize();