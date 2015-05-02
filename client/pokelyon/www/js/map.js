var Map = {
	create: function(options) {

		this.map = L.map('map').setView([45.757927, 4.847598],15);
		this.map.locate({setView: true, maxZoom:16});

		this.layer = L.tileLayer('https://{s}.tiles.mapbox.com/v3/examples.map-i875mjb7/{z}/{x}/{y}.png', {
			attribution: 'Map data &copy; <a href="http://openstreetmap.org">OpenStreetMap</a> contributors, <a href="http://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="http://mapbox.com">Mapbox</a>',
	    	maxZoom: 18
		});

		this.layer.addTo(this.map);
	},

	onClick: function( callback ) {
		this.map.addEventListener('click',callback);
	},

	offClick: function( callback ) {
		this.map.removeEventListener('click',callback);
	}
};

document.addEventListener('deviceready', function(e) {
	
	Map.create();

	Map.onClick(function(click){
		console.log(click.latlng.lat, click.latlng.lng,click.layerPoint.x,click.containerPoint.x);
		Map.offClick();
	});
});

$(window).on('hashchange', function() {
  	var hash = location.hash;
  	switch( hash ) {
  		case '#menu':
  			var width = $('#menugui').css("width");

  			$('#menugui').transition({x:'-'+width});
  			break;
  		case '#directions':
  			var height = $('#directionsgui').css("height");
  			$('#directionsgui').transition({y:'-'+height});
  			break;
  		default:
  			$('#menugui').transition({x:0});
  			$('#directionsgui').transition({y:0});
  			break;
  	}
});