
/* usage example :
	var r = parseData(buffer);
	for(var i=0;i<r.length;++i){
		// do something smart with r[i].x and r[i].y
	}
*/
function parseData(buffer) {
	var sizeview = new Uint32Array(buffer);
	var size = sizeview[1];
	var size_buf = (buffer.byteLength-8)/8/2;
	//alert("size: "+size+"\n buffer: "+size_buf + "\ntotal buffer size : " + buffer.byteLength);
	buffer = buffer.slice(8);
	var sizeview = new Float64Array(buffer);
	var path = [];
	for(var i=0;i<size_buf;++i){
		var p = {x:sizeview[2*i],y:sizeview[2*i+1]};
		path.push(p);
	}
	
	// var str = "";
	// for(var i = 0; i < path.length; ++i)
	// {
	// 	str += i += ") " + path[i].x + " " + path[i].y + '\n';
	// }
	// alert(str);

	return path;
}

function parseData2(buffer) {
	var sizeview = new Float64Array(buffer);
	var path = [];
	for(var i=0;i<buffer.byteLength/8/2;++i){
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
	alert(path.length);
	var pointList = [];
	for(var i=0;i<path.length;++i){
		if(path[i].x !== undefined){
			pointList.push([path[i].x,path[i].y]);
		}
	}
	if(path[0].x !== undefined)
		L.marker([path[0].x,path[0].y]).addTo(map);
	if(path[path.length-1] !== undefined)
		L.marker([path[path.length-1].x,path[path.length-1].y]).addTo(map);
	
	var polyline = L.polyline(pointList, {color: 'blue',
		opacity: 0.9,
		smoothFactor: 1 }).addTo(map);

	L.marker([45.7703, 4.87558]).addTo(map);
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
	var sizeview = new Uint32Array(data);
	var vtype = sizeview[0];
	var vsize = sizeview[1];
	return {type:vtype,size:vsize};
}

var abuffer = [];
function magicTcpReceive(adata,fun) {
	
	//console.log('Received SIZE: ' + adata.byteLength );

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
