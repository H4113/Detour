
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
	var sizeview = new Uint32Array(buffer);
	var size = sizeview[1];
	var size_buf = (buffer.byteLength-8)/8/2;
	buffer = buffer.slice(8);
	var sizeview = new Float64Array(buffer);
	var path = [];
	for(var i=0;i<size_buf;++i){
		var p = {x:sizeview[2*i],y:sizeview[2*i+1]};
		path.push(p);
	}

	return path;
}


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
