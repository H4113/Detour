
/* usage example :
	var r = parseData(buffer);
	for(var i=0;i<r.length;++i){
		// do something smart with r[i].x and r[i].y
	}
*/
function parseData(buffer) {
	var sizeview = new Uint32Array(buffer);
	var size = sizeview[0];
	
	buffer = buffer.slice(8);
	var sizeview = new Float64Array(buffer);
	var path = [];
	for(var i=0;i<size;++i){
		var p = {x:sizeview[2*i],y:sizeview[2*i+1]};
		path.push(p);
	}
	
	return path;
}