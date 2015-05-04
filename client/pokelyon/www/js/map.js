var circlePosUser = null;
var interval_GPS;
var bool_gps = true;

var Map = {
	create: function(options) {

		this.map = L.map('map').setView([45.757927, 4.847598],15);
		this.map.locate({setView: true, maxZoom:100});

		this.layer = L.tileLayer('https://{s}.tiles.mapbox.com/v3/examples.map-i875mjb7/{z}/{x}/{y}.png', {
			attribution: 'Map data &copy; <a href="http://openstreetmap.org">OpenStreetMap</a> contributors, <a href="http://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="http://mapbox.com">Mapbox</a>',
	    	maxZoom: 18
		});

		this.layer.addTo(this.map);

		var mapevt = H.createEvent('map-created',{Map:this});
		document.dispatchEvent( mapevt );
	},

	onClick: function( callback ) {
		this.map.addEventListener('click',callback);
	},

	offClick: function( callback ) {
		this.map.removeEventListener('click',callback);
	}
};

// On click Popup
document.addEventListener('map-created', function(e) {

	var popup = L.popup();
			
	e.Map.onClick(function(click){
		popup.setLatLng(click.latlng)
			 .setContent("<a href='#go?&lat="+click.latlng.lat+"&lng="+click.latlng.lng+"' class='gps-btn'>Aller ici</a>")
			 .openOn(e.Map.map);
	});
});

document.addEventListener('deviceready', function(e) {
	
	Map.create();

	//alert('deviceready');

	circlePosUser = L.circleMarker([0, 0], 10, {}).addTo(Map.map);

	var onGeoSuccess = function(position) {
		circlePosUser.setLatLng([position.coords.latitude, position.coords.longitude]);
		circlePosUser.redraw();
	};

	// onError Callback receives a PositionError object
	//
	function onGeoError(error) {
		if(bool_gps)
			alert("No GPS");
		clearInterval(interval_GPS);
		bool_gps = false;
	}

	interval_GPS = setInterval(function () {
		navigator.geolocation.getCurrentPosition(onGeoSuccess, onGeoError);
	}, 500);

	document.dispatchEvent( H.createEvent('hashchange') );
});

$(window).on('hashchange', function() {
  	var hash = location.hash;
  	var index = hash.indexOf('?');
  	if( index > -1 ) hash = hash.substring(0,index);

  	switch( hash ) {
  		case '#menu':
  			var width = $('#menugui').css("width");

  			$('#menugui').transition({x:'-'+width});
  			break;
  		case '#directions':
  			var height = $('#directionsgui').css("height");
  			$('#directionsgui').transition({y:'-'+height});
  			break;
  		case '#go':
  			{
  				var close = $(".leaflet-popup-close-button")[0];
  				if(close)close.click();
  			}
  			break;
  		default:
  			$('#menugui').transition({x:0});
  			$('#directionsgui').transition({y:0});
  			break;
  	}
});