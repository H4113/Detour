
/* usage example :
	var r = parseData(buffer);
	for(var i=0;i<r.length;++i){
		// do something smart with r[i].x and r[i].y
	}
*/

var map_start = null;
var map_destination = null;
var map_path = null;

function parseData(buffer) {
	
	// Header
	var view = new Uint32Array(buffer,0,4);
	var size = view[1];
	var size_path = view[2];
	var size_touri = view[3];
	
	// Coords
	buffer = buffer.slice(view.byteLength);
	view = new Float64Array(buffer,0,size_path*2);
	
	var path = [];
	for(var i=0;i<size_path;++i){
		var p = {x:view[2*i],y:view[2*i+1]};
		path.push(p);
	}
	
	return path;
	
	var NB_STRING = 5;
	var touri = [];
	// Touri size
	console.log(view.byteLength);
	buffer = buffer.slice(view.byteLength);
	view = new Int16Array(buffer,0,size_touri*NB_STRING);
	for(var i=0;i<size_touri;++i){
		var tab = [];
		for(var j=0;j<NB_STRING;++j){
			tab[j] = view[5*i+j];
			if(tab[j] < 0){
				tab[j] = 0;
			}
		}
		touri.push({size:tab,string:[],x:0,y:0});
	}
	
	// Touri coord
	buffer = buffer.slice(view.byteLength);
	view = new Float64Array(buffer,0,size_touri*2);
	for(var i=0;i<size_touri;++i){
		touri[i].x = view[2*i];
		touri[i].y = view[2*i+1];
	}
	
	// Touri string
	/*buffer = buffer.slice(view.byteLength);
	for(var i=0;i<size_touri;++i){
		for(var j=0;j<NB_STRING;++j){
			view = new Uint8Array(buffer,0,touri[i].size[j]);
			buffer.slice(view.byteLength);
			touri[i].str[j] = String.fromCharCode.apply(null, view);
		}
	}*/
	
	for(var i=0;i<size_touri;++i){
		console.log("TOURI "+i);
		console.log("coord: "+touri[i].x+" "+touri[i].y);
		for(var j=0;j<NB_STRING;++j){
			console.log(" "+j+") "+touri[i].size[j]);
			//console.log(" "+j+") "+touri[i].str[j]);
		}
	}
	console.log(JSON.stringify(touri));

	return path;
}
/*
int 32 * 4 // type size path_size touri_size
double * path_size
	int_16 * 5
	string * 5			--> * touri_size
	double * 2


/* usage :
	var path = parseData(buffer);
	drawPathOnMap(map, path);
*/
function drawPathOnMap(map, path){
	if(map_path != null)
		map.removeLayer(map_path);
	if(map_start != null)
		map.removeLayer(map_start);
	if(map_destination != null)
		map.removeLayer(map_destination);

	var pointList = [];
	for(var i=0;i<path.length;++i){
		if(path[i].x !== undefined){
			pointList.push([path[i].x,path[i].y]);
		}
	}
	if(path[0].x !== undefined) {
		map_start = L.marker([path[0].x,path[0].y]).addTo(map);
		map_start.addTo(map);
	}
	if(path[path.length-1] !== undefined) {
		map_destination = L.marker([path[path.length-1].x,path[path.length-1].y]);
		map_destination.addTo(map);
	}
	
	map_path = L.polyline(pointList, {color: 'blue',
		opacity: 0.9,
		smoothFactor: 1 });
	map_path.addTo(map);

	//map.fitBounds(polyline.getBounds());
}

// ---
function concatAbuf(buf,data){
	var tmp = new Uint8Array(data);
	var buf2 = new Uint8Array(buf.buffer,buf.pos,data.byteLength);
	buf.pos += data.byteLength;
	buf2.set(tmp);
	return buf;
}


function readHeader(data){
	if(data.byteLength > 0){
		var sizeview = new Uint32Array(data,0,2);
		var vtype = sizeview[0];
		var vsize = sizeview[1];
		return {type:vtype,size:vsize};
	}else{
		return {type:0,size:0};
	}
}

function magicTcpReceive(abuffer, adata,fun) {

	if(abuffer.length == 0){
		var header = readHeader(adata);
		size_packet = header.size;
		abuffer[0] = {buffer :new ArrayBuffer(header.size),pos: 0,type:header.type};
	}
	
	var i = 0;
	while(abuffer[0].pos+adata.byteLength >= abuffer[0].buffer.byteLength){
		var slice_pos = abuffer[0].buffer.byteLength-abuffer[0].pos;
		var realdata = adata.slice(0,slice_pos);
		abuffer[0] = concatAbuf(abuffer[0],realdata);
		
		fun(abuffer[0]);
		abuffer.shift();
		
		adata = adata.slice(slice_pos,adata.byteLength);
		if(adata.byteLength <= 0){
			break;
		}
		var header2 = readHeader(adata);
		abuffer[0] = {buffer: new ArrayBuffer(header2.size),pos:0,type:header2.type};
	}
	if(adata.byteLength > 0 && adata.byteLength >= 0){
		abuffer[0] = concatAbuf(abuffer[0],adata);
	}
	
}


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

//addressToCoordinates('4 rue Armand', function(coords){ //Call a function using coords });
function addressToCoordinates(address, ondone) {
	var access_token = "pk.eyJ1IjoiaDQxMTMiLCJhIjoib3JScEdYMCJ9.hwB8vIlfpiQh49pkk8YRCA";
	var city = "+Lyon+France";
	var web = "http://api.tiles.mapbox.com/v4/geocode/mapbox.places/";
	var web_end = ".json?access_token=";

	address = address.split(' ').join('+');

	var req = new XMLHttpRequest();
	var url =  web+address+city+web_end+access_token;

	$.getJSON(url, function(json) {
		var coords = json['features'][0].center;
		ondone(coords);
	});
}
