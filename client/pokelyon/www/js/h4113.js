
var H = {

	createEvent: function(name, properties) {
		var evt = document.createEvent("Event");
		evt.initEvent(name,true,true);
		if( properties ) {
			for( var p in properties ) {
				evt[p] = properties[p];
			}
		}
		return evt;
	},

	processData: function (obj){
		//alert("packet entier, taille: "+obj.buffer.byteLength);
		var data = obj.buffer;
		if(obj.type == 1){ // type == 1 -> PATH sent
			var path = parseData(data);
			return path;
			drawPathOnMap(Map.map, path);
		}
	},

	sendQuery: function (buf, callback, error) {
		chrome.socket.create('tcp', {}, function(createInfo) {
			
			var abuffer = [];
			var socketId = createInfo.socketId;
			function readPackets(readInfo) {
				//alert("packet partiel, taille:"+readInfo.data.byteLength);
				magicTcpReceive(abuffer, readInfo.data, function(obj){callback(processData(obj))});
				chrome.socket.read(socketId, null, readPackets);
			}
			chrome.socket.connect(socketId, "151.80.143.42", 6666, function(result) {
			//chrome.socket.connect(socketId, "192.168.1.199", 6666, function(result) {
				if(result >= 0) {
					//alert("prout " + result);
					chrome.socket.write(socketId, buf, function(writeInfo) {
						if(writeInfo.bytesWritten > 0) {
							//alert("ouiiiii ");
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
	},

	requestWay: function( params, callback, error ) {
		
		if( ! params.fromlat ) throw 'required fromlat param is missing.';
		if( ! params.fromlng ) throw 'required fromlng param is missing.';
		if( ! params.tolat ) throw 'required tolat param is missing.';
		if( ! params.tolng ) throw 'required tolng param is missing.';

		var buf = new ArrayBuffer(8+4*8);
		var type_req = new Int16Array(buf,0,1);
		var type_junk = new Int16Array(buf,2,1);
		var type_junk2 = new Int32Array(buf,4,1);
		var gpscoord = new Float64Array(buf,8,4);
		
		type_req[0] = 0;
		type_junk[0] = 42;
		type_junk2[0] = 42;

		gpscoord[0] = params.fromlat;
		gpscoord[1] = params.fromlng;
		gpscoord[2] = params.tolat;
		gpscoord[3] = params.tolng;

		// Change when buttons available
		type_junk[0] = 1 + (1 << 1) + (1 << 2);

		sendQuery(buf);
	} 


};