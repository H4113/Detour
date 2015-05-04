var circlePosUser = null;

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

	circlePosUser = L.circleMarker([0, 0], 10, {}).addTo(Map.map);

	var onGeoSuccess = function(position) {
		circlePosUser.setLatLng([position.coords.latitude, position.coords.longitude]);
		circlePosUser.redraw();
	};

	// onError Callback receives a PositionError object
	//
	function onGeoError(error) {
		alert("No GPS");
	}

	setInterval(function () {
		navigator.geolocation.getCurrentPosition(onGeoSuccess, onGeoError);
	}, 500);

	document.dispatchEvent( H.createEvent('hashchange') );
});

var State = {
	last: null,
	defaultState: null,

	launch: function( state ) {
		if( ! this.states[state] ) throw state + ' is not a valid state name.';
		if( this.states[state].init ) {
			this.states[state].init();
		} else {
			this.clear();
		}
		if( this.states[state].launch ) this.states[state].launch();
		this.last = state;
	},
	clearLast: function() {
		if( ! this.last ) {
			//if( ! this.defaultState ) throw 'missing default state to clear';
			//this.last = this.defaultState;
			return;
		}
		this.clear(this.last);
	},
	clear: function( state ) {
		if( ! state ) return this.clearLast();
		if( ! this.states[state] ) throw state + ' is not a valid state name.';
		if( this.states[state].clear ) this.states[state].clear();
	},

	states : {},

	hashTable : {},

  	hashToState : function( hash ) {
  		if( this.hashTable[hash] ) {
  			return this.hashTable[hash];
  		}
  		return this.defaultState;
  	},

  	setDefaultState : function( stateName ) {
  		this.defaultState = stateName;
  	},

  	voidFunc: function(){},

  	addState : function( name, hash, init, launch, clear){

  		this.hashTable[hash] = name;
  		var state = {
  			init: init,
  			launch: launch,
  			clear: clear
  		};
  		this.states[name] = state;

  		if( ! this.defaultState ) {
  			this.defaultState = name;
  			this.last = name;
  		}

  		return this;
  	}
};

State
	.addState('home','#', null )
	.addState('itinary','#directions', null,
		function() {
			H.jQueryMoveTopLeft('#directionsgui');
		},
		function() {
			H.jQueryResetPos('#directionsgui');
		})
	.addState('menu','#menu', null,
		function() {
			H.jQueryMoveTopLeft('#menugui');
		},
		function() {
			H.jQueryResetPos('#menugui');
		})
	.addState('go','#go', null,
		function() {
			var close = $(".leaflet-popup-close-button")[0];
  			if(close)close.click();
		});

$(window).on('hashchange', function() {
  	var hash = location.hash;
  	var index = hash.indexOf('?');
  	if( index > -1 ) hash = hash.substring(0,index);

  	State.launch( State.hashToState(hash) );
});