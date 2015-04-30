/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.	See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.	 See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

 function str2ab(str) {
	var buf = new ArrayBuffer(str.length);
	var bufView = new Uint8Array(buf);
	for (var i=0, strLen=str.length; i<strLen; i++) {
		bufView[i] = str.charCodeAt(i);
	}
	return buf;
}

function arrayBufferToString(buffer) {
	return String.fromCharCode.apply(null, new Uint8Array(buffer));
}

function sendQuery(buf) {
	chrome.socket.create('tcp', {}, function(createInfo) {
		//alert(createInfo);
		var socketId = createInfo.socketId;
		chrome.socket.connect(socketId, "192.168.1.199", 6666, function(result) {
			if(result >= 0) {
				alert("prout " + result);
				chrome.socket.write(socketId, buf, function(writeInfo) {
					if(writeInfo.bytesWritten > 0) {
						alert("ouiiiii ");
						
						// NEW
						// chrome.socket.read(socketId, 8, function(readInfo) {
						// 	alert("recu init: "+readInfo.data.byteLength);
						// 	var dataview = new Uint32Array(readInfo.data);
						// 	var type = dataview[0];
						// 	var size = dataview[1];
						// 	var buf = new ArrayBuffer(size);
							
							
						// 	// read all coordinates
						// 	var current_pos = 0;
						// 	function readCoord(readInfo) {
      //           alert("bite");
						// 		var tmp = new Float64Array(readInfo.data);
						// 		var buf2 = new Float64Array(buf,current_pos,readInfo.data.byteLength);
						// 		current_pos += readInfo.data.byteLength;
						// 		buf2.set(tmp);
								
						// 		if(readInfo.data.byteLength == 0){
						// 			alert("fin prematuree");
						// 			current_pos = size; // security
						// 		}

      //           if(current_pos<size) {
      //             chrome.socket.read(socketId, null, readCoord);
      //           }
						// 	}
							
						// 	chrome.socket.read(socketId, null, readCoord);
						// 	while(current_pos < size){
						// 		// wait;
						// 	}
						// 	alert("recu fin");
						// 	//alert(arrayBufferToString(readInfo.data));
						// 	var path = parseData2(buf);
						// 	var str = "";
						// 	for(var i=0;i<path.length;i++){
						// 		if(path[i] !== undefined) {
						// 			str += i+") "+path[i].x+" "+path[i].y+"\n";
						// 		}
						// 	}

						// 	alert(str);
						// 	drawPathOnMap(Map.map,path);
						// });
						// NEW 
						/*
						chrome.socket.read(socketId, null, function(readInfo) {
							alert("recu");
							//alert(arrayBufferToString(readInfo.data));
							var path = parseData(readInfo.data);
							var str = "";
							for(var i=0;i<path.length;i++){
								if(path[i] !== undefined) {
									str += i+") "+path[i].x+" "+path[i].y+"\n";
								}
							}

							alert(str);
							drawPathOnMap(Map.map,path);
						});*/
						//alert("TRALALALALALA");
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
			//alert(gpscoord[1]);
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