var circlePosUser = null;
var interval_GPS;
var bool_gps = true;

///////////////////////////////////////////////////////////////////////////////////
//                               STATE
///////////////////////////////////////////////////////////////////////////////////

// --> Used to manage the states the app can be in
// --> States are associated to a unique hastag


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
		if( this.last ) this.clear(this.last);
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

var Map = {
	create: function(options) {

		this.map = L.map('map').setView([45.757927, 4.847598],15);
		this.map.locate({setView: true, maxZoom:100});

		this.layer = L.tileLayer('https://{s}.tiles.mapbox.com/v3/examples.map-i875mjb7/{z}/{x}/{y}.png', {
			attribution: 'Map data &copy; <a href="http://openstreetmap.org">OpenStreetMap</a> contributors, <a href="http://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="http://mapbox.com">Mapbox</a>',
	    	maxZoom: 18,
	    	errorTileUrl: 'img/notile.jpg'
		});

		this.layer.addTo(this.map);

		var mapevt = H.events.create('map-created',{Map:this});
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
		if( H.user.locationKnown() ){
			popup.setLatLng(click.latlng)
			 /*.setContent("<a href='?fromlat="
			 	+H.user.lastKnownLocation.lat
			 	+"&fromlng="+H.user.lastKnownLocation.lng
			 	+"&tolat="+click.latlng.lat
			 	+"&tolng="+click.latlng.lng
			 	+"#go' class='gps-btn'>Aller ici</a>")*/
			 .setContent("<a href='#go"+
			 	H.objToUrl(H.makeItinaryObj(
			 	H.user.lastKnownLocation.lat,
			 	H.user.lastKnownLocation.lng,
			 	click.latlng.lat,
			 	click.latlng.lng))
			 	+"' class='gps-btn'>Aller ici</a>")
			 .openOn(e.Map.map);
		}
	});
});

document.addEventListener('deviceready', function(e) {
	
	Map.create();

	circlePosUser = L.circleMarker([0, 0], 10, {}).addTo(Map.map);

	var onGeoSuccess = function(position) {
		H.user.updateLocation(position.coords.latitude, position.coords.longitude);
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

	document.dispatchEvent( H.events.create('hashchange') );

});


// States of the app
function getParameterByName(name) {
    var url = location.href.substring(location.href.lastIndexOf('?')+1/*,location.href.lastIndexOf('#')*/);
    var params = url.split('&');
    for( var i = 0; i < params.length; ++i) {
    	var regex = new RegExp( name +"=(.*)");
    	var results = regex.exec( params[i] );
    	if( results ) {
    		return results[1];
    	}
    }
    return '';
}

State
.addState('home','#', null )
.addState('itinary','#directions', null,
	// launch
	function() {
		H.gui.reveal('#directionsgui');
	},
	// clear
	function() {
		H.jQuery.resetPos('#directionsgui');
	})
.addState('menu','#menu', null,
	// launch
	function() {
		H.gui.reveal('#menugui');
	},
	// clear
	function() {
		H.jQuery.resetPos('#menugui');
	})
.addState('go','#go', null,
	// launch
	function() {
		var close = $(".leaflet-popup-close-button")[0];
		if(close) {
			close.dispatchEvent( H.events.create('click') );
		}

		var params = {
			fromlat: getParameterByName('fromlat'),
			fromlng: getParameterByName('fromlng'),
			tolat: getParameterByName('tolat'),
			tolng: getParameterByName('tolng'),
			patrimony: parseInt(getParameterByName('patrimony')),
			gastronomy: parseInt(getParameterByName('gastronomy')),
			accomodity: parseInt(getParameterByName('accomodity')),
			hours: parseInt(getParameterByName('hours')),
			minutes: parseInt(getParameterByName('minutes')),
			onMap: (getParameterByName('onMap') === 'true')
		};

		H.requestWay( params, function(path){drawPathOnMap(Map.map, path);},function(){});
	});



$(window).on('hashchange', function() {
  	var hash = location.hash;
  	var index = hash.indexOf('?');
  	if( index > -1 ) hash = hash.substring(0,index);

  	State.launch( State.hashToState(hash) );
});

$(window).on('resize', function(){
	switch( State.last ) {
	case 'itinary':
		H.gui.reveal('#directionsgui');
		break;
	case 'menu':
		H.gui.reveal('#menugui');
		break;
	}
});