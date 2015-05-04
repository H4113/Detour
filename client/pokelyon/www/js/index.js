


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
		/*var buf = new ArrayBuffer(8+4*8);
		var type_req = new Int16Array(buf,0,1);
		var type_junk = new Int16Array(buf,2,1);
		var type_junk2 = new Int32Array(buf,4,1);
		var gpscoord = new Float64Array(buf,8,4);
		
		type_req[0] = 0;
		type_junk[0] = 42;
		type_junk2[0] = 42;*/
	
		var params = {
			fromlat: null,
			fromlng: null,
			tolat: null,
			tolng: null
		};

		var onSuccess = function(position) {
			/*alert('Latitude: '			  + position.coords.latitude		  + '\n' +
					  'Longitude: '			+ position.coords.longitude			+ '\n' +
					  'Altitude: '			+ position.coords.altitude			+ '\n' +
					  'Accuracy: '			+ position.coords.accuracy			+ '\n' +
					  'Altitude Accuracy: ' + position.coords.altitudeAccuracy	+ '\n' +
					  'Heading: '			+ position.coords.heading			+ '\n' +
					  'Speed: '				+ position.coords.speed				+ '\n' +
					  'Timestamp: '			+ position.timestamp				+ '\n');*/
			params.fromlat = position.coords.latitude;
			params.fromlng = position.coords.longitude;
			H.sendQuery(params, function(path){drawPathOnMap(Map.map, path);},function(){});
		};
	
		// onError Callback receives a PositionError object
		//
		function onError(error) {
			alert('code: '	+ error.code	+ '\n' +
				'message: ' + error.message + '\n');
		}
	
		Map.map.on('dblclick', function(e) {
			params.tolat = e.latlng.lat;
			params.tolng = e.latlng.lng;

			// Change when buttons available
			//type_junk[0] = 1 + (1 << 1) + (1 << 2);

			navigator.geolocation.getCurrentPosition(onSuccess, onError);
		});
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