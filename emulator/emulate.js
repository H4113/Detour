/*
* Client emulator
*/

// includes ------------------------------------------------------------------

var net = require('net');
var fs = require('fs');
var events = require('events');
var WebSocket = require('ws')
var eventclient = new events.EventEmitter();

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


function isNumber(n) {
	return !isNaN(parseFloat(n)) && isFinite(n);
}


// ----------------------------------------------------------------------------

var option_nojson = false;

// ---

// create fake data
function getData(){
	// GPS coords:
	//var datagps = [45.760574,4.8593929999999546,45.7514164745,4.82062964108];
	var datagps = [45.782741199999996695169102167711, 4.873665800000000380975961888907, 45.782249782982248120788426604122, 4.878101348876953125000000000000];
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
		if(!option_nojson)
			objtofile(path,"data/path.json");
		//drawPathOnMap(Map.map, path);
	}
}

// socket managment : ---------------------------------------------------------

var clientstate;


function launchClient(id){
	
	var client = new net.Socket();
	var abuffer = [];
	
	var starttime;
	var endstate = 0;

	client.connect(PORT,IP, function() {
		console.log('Connected to '+IP+":"+PORT);
		var data = getData();
		data = toBuffer(data);
		try {
			client.write(data);
			starttime = process.hrtime();
			client.setTimeout(30*1000);  
		} catch(e) {
			console.log("EXCEPTION: "+e);
		}
		client.end();
	});


	client.on('data', function(data){
		magicTcpReceive(abuffer, toArrayBuffer(data),processData );
		if(abuffer.length == 0){
			eventclient.emit('time',id,process.hrtime(starttime));
		}
	});
	 
	client.on('close', function() {
		eventclient.emit('end',id,endstate);
		console.log('Connection closed');
	});
	
	client.on('timeout', function() {
		/*endstate = 2;
		client.emit('error','timeout');
		
		client.destroy();*/
	});

	client.on('error', function(e) {
		if(endstate == 0)
			endstate = 1;
		console.log('ERROR '+e);
	});

}

function launchClientWS(id){
	starttime = process.hrtime();
	var nbtry = 0;
	var url = 'ws://'+IP+'/';
	var client = new WebSocket(url);
	var abuffer = [];
	
	var starttime;
	var endstate = 0;

	function connect(){
		console.log('Connected to '+IP+":"+PORT+' ('+id+')');
		
		var data = getData();
		data = toBuffer(data);
		try {
			client.send(data);
		} catch(e) {
			console.log("EXCEPTION: "+e);
		}
		
		client.on('message', function (data) {
			magicTcpReceive(abuffer, toArrayBuffer(data),processData );
			if(abuffer.length == 0){
				eventclient.emit('time',id,process.hrtime(starttime));
				//eventclient.emit('end',id,endstate);
				client.close();
			}
		}); 
		
		client.on('close', function () {
			if(endstate == 0)
				eventclient.emit('end',id,endstate);
			console.log('Connection closed');
		}); 
		
	};
	
	
	client.on('open', connect);
	
	client.on('error', function(e) {
		++nbtry;
		endstate = 1;
		console.log('ERROR ('+id+') '+e);
		if(nbtry<100){
			console.log('retry '+id);
			client = new WebSocket(url);
			client.on('open', connect);
		}else
			eventclient.emit('end',id,1);
	});

}

function showHelp(){
	console.log("Usage:");
	console.log("\tnode emulate.js <nb client>");
	console.log("\tnode emulate.js [Option]");
	console.log("");
	console.log("Options");
	console.log("\tclean : clean data/benchmark.json");
	console.log("\t--help or -h : show this help");
}

function main(){
	var nbend = 0;
	var nbclient = 1;
	var databenchmark = [];
	var statebench = 0;
	
	process.argv.forEach(function (val, index, array) {
		if(index >= 2 && (val == "--help" || val == "-h")){
			showHelp();
			process.exit(0);
		}
		if(index == 2){
			if(isNumber(val)){
				option_nojson = true;
				nbclient = val;
			}else if(val == "clean"){
				fs.writeFile("data/benchmark.json", "[]", function(err) {
					if(err) return console.log(err);
					else console.log("cleaning benchmark data done !");
					process.exit(0);
				}); 
			}
		}
	});
	
	eventclient.on('end', function(id,state){
		++nbend;
		databenchmark[id].state = state;
		if(state != 0){
			statebench = 1;
		}
		if(nbend >= nbclient ){
			console.log('end, writing data');
			var bench = JSON.parse(fs.readFileSync('data/benchmark.json', 'utf8'));
			if(bench[nbclient] === undefined || bench[nbclient] === null)
				bench[nbclient] = [];
			bench[nbclient].push(databenchmark);
			objtofile(bench,"data/benchmark.json");
		}
	});
	
	eventclient.on('time', function(id,time){
		databenchmark[id].time = time[0]+time[1]/1000000000;
		console.log('time '+id+' : '+time+' ');
	});
	
	
	
	console.log('launch '+nbclient+' client(s)');
	for(var i=0;i<nbclient;++i){
		databenchmark[i] = {state:-1,time:0};
		launchClientWS(i);
	}
	
	eventclient.on('error', function(e) {
			console.log('ERROR cli '+e);
		});
	
	process.on('uncaughtException', function (err) {
		console.log('Caught exception: ' + err);
		if(err.code == 'ETIMEDOUT'){
			console.log('retry'+i);
			//launchClientWS(i);
		}
	});

}

main();