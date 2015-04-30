var Map = {
	create: function() {
		this.map = L.map('map').setView([45.757927, 4.847598], 15);

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