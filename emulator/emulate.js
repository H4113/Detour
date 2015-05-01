/*
* Client emulator
*/

// includes ------------------------------------------------------------------

var net = require('net');
var fs = require('fs');

// hack to avoid the error "alert is not defined"
eval.apply(global, ["function alert(v){}"]);

function read(f) {
	return fs.readFileSync(f).toString();
}
function include(f) {
	eval.apply(global, [read(f)]);
}

include('../client/pokelyon/www/js/utils.js');
include('config.js');


// utils ---------------------------------------------------------------------

function toBuffer(ab) {
    var buffer = new Buffer(ab.byteLength);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buffer.length; ++i) {
        buffer[i] = view[i];
    }
    return buffer;
}

function toArrayBuffer(buffer) {
    var ab = new ArrayBuffer(buffer.length);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buffer.length; ++i) {
        view[i] = buffer[i];
    }
    return ab;
}

function objtofile(obj,outputFilename){
	fs.writeFile(outputFilename, JSON.stringify(obj, null), function(err) {
		if(err) {
			console.log(err);
		} else {
			console.log("JSON saved to " + outputFilename);
		}
	}); 
}

// ----------------------------------------------------------------------------

// create fake data
function getData(){
	// GPS coords:
	var datagps = [45.760574,4.8593929999999546,45.7514164745,4.82062964108];
	var buf = new ArrayBuffer(8+8*datagps.length);
	var sz = new Uint16Array(buf, 0, 4);
	sz[0] = 0;
	sz[1] = 0;
	var p = new Float64Array(buf, 8, datagps.length);
	p.set(datagps);
	return buf;
}

// do something with the received data
function processData(obj){
	console.log("packet entier, taille: "+obj.buffer.byteLength);
	var data = obj.buffer;
	if(obj.type == 1){ // type == 1 -> PATH sent
		var path = parseData(data);
		objtofile(path,"data/path.json");
		//drawPathOnMap(Map.map, path);
	}
}

// socket managment : ---------------------------------------------------------

var client = new net.Socket();

client.connect(PORT,IP, function() {
	console.log('Connected to '+IP+":"+PORT);
	var data = getData();
	data = toBuffer(data);
	try {
		client.write(data);
	} catch(e) {
		console.log("EXCEPTION: "+e);
	}
	client.end();
});


client.on('data', function(data){
	magicTcpReceive( toArrayBuffer(data),processData );
});

 
client.on('close', function() {
	console.log('Connection closed');
});

client.on('error', function(e) {
	console.log('ERROR '+e);
});

