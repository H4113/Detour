
/* usage example :
	var r = parseData(buffer);
	for(var i=0;i<r.length;++i){
		// do something smart with r[i].x and r[i].y
	}
*/
function parseData(buffer) {
	var sizeview = new Uint32Array(buffer);
	var size = sizeview[1];
	buffer = buffer.slice(8);
	var sizeview = new Float64Array(buffer);
	var path = [];
	for(var i=0;i<size;++i){
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
	var pointList = [];
	for(var i=0;i<r.length;++i){
		pointList.push([r[i].x,r[i].y]);
	}
	
	L.marker([r[0].x,r[0].y]).addTo(map);
	L.marker([r[r.length-1].x,r[r.length-1].y]).addTo(map);
	
	var polyline = L.polyline(pointList, {color: 'blue',
		opacity: 0.9,
		smoothFactor: 1 }).addTo(map);
}