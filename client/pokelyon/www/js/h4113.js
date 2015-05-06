
var H = {

	//todo : H.user.nananana, H.events.createEvent, etc...

	//   USER RELATED
	user : {
		lastKnownLocation: {},

		locationKnown: function() {
			return this.lastKnownLocation.lat && this.lastKnownLocation.lng ;
		},

		updateLocation: function( a, b ) {
			switch( arguments.length ) {
			case 0:
				throw 'H.user.updateLocation : at least one argument is required';
				break;
			case 1:
				if( ! a.lat ) throw 'H.user.updateLocation : lat property is required';
				if( ! a.lng ) throw 'H.user.updateLocation : lng property is required';
				this.lastKnownLocation.lat = a.lat;
				this.lastKnownLocation.lng = a.lng;
			case 2:
				this.lastKnownLocation.lat = a;
				this.lastKnownLocation.lng = b;
				break;
			default:
				throw 'H.user.updateLocation : invalid number of parameters.';
				break;
			}
		}
	},

	//   JQUERY RELATED
	jQuery: {
		moveTopLeft: function(selector) {
			//var offy = $(selector).offset().top +1;
	  		var offx = $(selector).offset().left +1;
	  		var offy = $(window).height() +1;
	  		//var offx = $(window).width() +1;
	  		$(selector).transition({y:'-'+offy, x:'-'+offx});
		},

		resetPos: function( selector ) {
			$(selector).transition({y: 0, x: 0});
		}
	},

	gui: {

		reveal: function(selector) {

			$(selector).each( function() {

				var x = 0,y =0;
				if( $(this).hasClass('hidden-bottom') ) {
					x = -1;
					y = -1 -$(window).height();

				} else if( $(this).hasClass('hidden-right') ) {
					x = -1 -$(window).width();
					y = -1;
				}

				$(this).transition({x:x,y:y});
			});
		}

	},

	//   EVENTS

	events: {

		dispatch: function( event ) {
			document.dispatchEvent(event);
		},

		create: function(name, properties) {
			var evt = document.createEvent("Event");
			evt.initEvent(name,true,true);
			if( properties ) {
				for( var p in properties ) {
					evt[p] = properties[p];
				}
			}
			return evt;
		},

		on: function( name, callback ) {
			document.addEventListener(name,callback);
		}

	},

	processData: function (obj){
		//alert("packet entier, taille: "+obj.buffer.byteLength);
		var data = obj.buffer;
		if(obj.type == 1){ // type == 1 -> PATH sent
			var obj2 = parseData(data);

			drawPathOnMap(Map.map, obj2.path);
			drawTourismOnMap(Map.map, obj2.tourism);
		}
	},

	sendQuery: function (buf, callback, error) {

		//this.sendMessage("Hellooooooo", null, null);

		chrome.socket.create('tcp', {}, function(createInfo) {
			
			var abuffer = [];
			var socketId = createInfo.socketId;
			function readPackets(readInfo) {
				//alert("packet partiel, taille:"+readInfo.data.byteLength);
				var prout = readHeader(readInfo.data);
				//alert(prout.size);
				magicTcpReceive(abuffer, readInfo.data, H.processData);
				chrome.socket.read(socketId, null, readPackets);
			}
			chrome.socket.connect(socketId, "151.80.143.42", 6666, function(result) {
			//chrome.socket.connect(socketId, "192.168.1.199", 6666, function(result) {
				//alert(result);
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
					// Cannot connectError during 
					// Do something smart
				}

			});
		});

	},

	// --> WebSocket --------------------------------------------------
	sendMessage: function( message, callback, error ) {
		var abuffer = [];
		//var ws = new WebSocket('ws://echo.websocket.org');
		var ws = new WebSocket('ws://192.168.56.101:80');
		
		ws.onopen = function (event) {
			ws.send( message, { binary: true} );
		};

		ws.onmessage = function(evt) {
			//console.log(evt.data);
			//alert( "answer  SIZE ");
			var arrayBuffer;
			var fileReader = new FileReader();
			fileReader.onload = function() {
				arrayBuffer = this.result;
				magicTcpReceive(abuffer, arrayBuffer, H.processData);
			};
			fileReader.readAsArrayBuffer(evt.data);
		};

		ws.onfragment = function(evt) {
			//alert("prout");
		}

		ws.onclose = function() {
			//alert('connection closed');
		};
	},
	

	requestWay: function( params, callback, error ) {
		
		if( ! params.fromlat ) throw 'required fromlat param is missing.';
		if( ! params.fromlng ) throw 'required fromlng param is missing.';
		if( ! params.tolat ) throw 'required tolat param is missing.';
		if( ! params.tolng ) throw 'required tolng param is missing.';
		if(!(typeof(params.patrimony) === 'number' )) throw 'required patrimony param is missing.';
		if(!(typeof(params.gastronomy) === 'number' )) throw 'required gastronomy param is missing.';
		if(!(typeof(params.accomodity) === 'number' )) throw 'required accomodity param is missing.';
		if(!(typeof(params.hours) === 'number' )) throw 'required hours param is missing.';
		if(!(typeof(params.minutes) === 'number' )) throw 'required minutes param is missing.';

		params.onMap = typeof params.onMap !== 'undefined' ? params.onMap : true;

		var buf = new ArrayBuffer(8+4*8);
		var type_req = new Int16Array(buf,0,1);
		var type_junk = new Int16Array(buf,2,1);
		var type_junk2 = new Int32Array(buf,4,1);
		var gpscoord = new Float64Array(buf,8,4);
		
		type_req[0] = 0;

		gpscoord[0] = params.fromlat;
		gpscoord[1] = params.fromlng;
		gpscoord[2] = params.tolat;
		gpscoord[3] = params.tolng;

		// Change when buttons available
		type_junk[0] = params.patrimony + (params.gastronomy << 1) + (params.accomodity << 2);

		//Change when time is available
		type_junk2[0] = params.hours*60 + params.minutes;
		
		var geo_sendQuery = function(position) {
			gpscoord[0] = position.coords.latitude;
			gpscoord[1] = position.coords.longitude;
			H.sendMessage(buf);
		};

		if(params.onMap) {
			navigator.geolocation.getCurrentPosition(geo_sendQuery, function() {});	
		}
		H.sendMessage(buf);
	},

	objToUrl : function( obj ) {
		var url = null;
		for( var prop in obj ) {
			if(  url ) {
				url += '&';	
			} else {
				url = '?';
			}
			url += prop +'='+obj[prop];
		}
		return url;
	},

	go : function( itinaryObj ) {
		if( ! itinaryObj.fromlat ) throw 'requested fromlat parameter missing.';
		if( ! itinaryObj.fromlng ) throw 'requested fromlng parameter missing.';
		if( ! itinaryObj.tolat ) throw 'requested tolat parameter missing.';
		if( ! itinaryObj.tolng ) throw 'requested tolng parameter missing.';
		if(!(typeof(itinaryObj.patrimony) === 'number' )) throw 'requested patrimony parameter missing.';
		if(!(typeof(itinaryObj.gastronomy) === 'number' )) throw 'requested gastronomy parameter missing.';
		if(!(typeof(itinaryObj.accomodity) === 'number' )) throw 'requested accomodity parameter missing.';
		if(!(typeof(itinaryObj.hours) === 'number' )) throw 'requested hours parameter missing.';
		if(!(typeof(itinaryObj.minutes) === 'number' )) throw 'requested minutes parameter missing.';
		window.location.hash = '#go' + this.objToUrl(itinaryObj);
	},

	makeItinaryObj: function( fromlat, fromlng, tolat, tolng, patrimony, gastronomy, accomodity, hours, minutes, onMap) {
		return {
			fromlat: fromlat,
			fromlng: fromlng,
			tolat: tolat,
			tolng: tolng,
			patrimony: patrimony,
			gastronomy: gastronomy,
			accomodity: accomodity,
			hours: hours,
			minutes: minutes,
			onMap : onMap
		};
	}

};

$("#itinaryForm").submit( function() {

	var from = document.getElementById('from').value;
	var to = document.getElementById('to').value;

	var gastro = document.getElementById('filter-food').checked ? 1 : 0;
	var heberg = document.getElementById('filter-night').checked ? 1 : 0;
	var patrim = document.getElementById('filter-tourism').checked ? 1 : 0;

	var hours = parseInt(document.getElementById('hours').value);
	var minutes = parseInt(document.getElementById('minutes').value);

	if(!hours) {
		hours = 23;
	}
	if(!minutes) {
		minutes = 0;
	}

	var isOk = true;

	if(!from) {
		var f = document.getElementById('alertFrom');
		f.style.display = 'block';
		$('#divFrom').addClass('has-error has-feedback');
		document.getElementById('errorSignFrom').style.display = 'block';

		isOk = false;
	}
	else {
		document.getElementById('alertFrom').style.display = 'none';
		$('#divFrom').removeClass('has-error has-feedback');
		document.getElementById('errorSignFrom').style.display = 'none';
	}
	if(!to) {
		document.getElementById('alertTo').style.display = 'block';
		$('#divTo').addClass('has-error has-feedback');
		document.getElementById('errorSignTo').style.display = 'block';

		isOk = false;
	}
	else {
		document.getElementById('alertTo').style.display = 'none';
		$('#divTo').removeClass('has-error has-feedback');
		document.getElementById('errorSignTo').style.display = 'none';
	}
	if(isOk) {
		convertFields(from, to, patrim, gastro, heberg, hours, minutes);	
	}
	return false;

	function convertFields(from, to, patrimony, gastronomy, accomodity, hours, minutes) {
		addressToCoordinates(from, function(coordsFrom){
			addressToCoordinates(to, function(coordsTo){
				var itinary = H.makeItinaryObj(coordsFrom.latitude,coordsFrom.longitude,
												coordsTo.latitude,  coordsTo.longitude, 
												patrimony, gastronomy, accomodity, hours, 
												minutes, false);
				H.go(itinary);
			});
		});
	}
});

H.events.on('globalization-enabled', function(evt) {
	$('.menu [type="checkbox"]').each( function() {
		$(this).bootstrapToggle({
	    	on: Lang.get('Yes'),
	    	off: Lang.get('No'),
	    	size: "small"
		});
	});
});
