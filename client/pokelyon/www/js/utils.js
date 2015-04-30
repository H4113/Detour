
/* usage example :
	var r = parseData(buffer);
	for(var i=0;i<r.length;++i){
		// do something smart with r[i].x and r[i].y
	}
*/
function parseData(buffer) {
	var sizeview = new Uint32Array(buffer);
	var size = sizeview[1];
	alert(size);
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
	for(var i=0;i<path.length;++i){
		if(path[i] !== undefined){
			pointList.push([path[i].x,path[i].y]);
		}
	}
	
	L.marker([path[0].x,path[0].y]).addTo(map);
	/*if(path[path.length-1] !== undefined)
		L.marker([path[path.length-1].x,path[path.length-1].y]).addTo(map);*/
	
	var polyline = L.polyline(pointList, {color: 'blue',
		opacity: 0.9,
		smoothFactor: 1 }).addTo(map);
	map.fitBounds(polyline.getBounds());
}