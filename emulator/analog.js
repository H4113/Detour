/*
* Client emulator
*/

// includes ------------------------------------------------------------------

var net = require('net');
var fs = require('fs');
var events = require('events');
var eventclient = new events.EventEmitter();
function read(f) {
	return fs.readFileSync(f).toString();
}
function include(f) {
	eval.apply(global, [read(f)]);
}



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

function getInfoTab(tab)
{
	var max = 0.0;
	var min = 0.0;
	var avg = 0.0;
	var count = 0;
	if(tab !== undefined)
	for(subtab of tab){
		for(val of subtab){
			if(val !== undefined){
				if(val.time > max)
					max = val.time;
				if(val.time < min)
					min = val.time;
				avg += val.time;
				++count;
			}
		}
	}
	avg /= count;
	return {max:max,min:min,avg:avg};
}


function main(){
	var bench = JSON.parse(fs.readFileSync('data/benchmark.json', 'utf8'));
	strfinal = "nb,min,max,avg\n"
	for(var nbuser=0;nbuser<bench.length;++nbuser){
		if(bench[nbuser] !== null && bench[nbuser].length > 0){
			strfinal += nbuser +",";
			var info = getInfoTab(bench[nbuser]);
			strfinal += info.min+",";
			strfinal += info.max+",";
			strfinal += info.avg;
			strfinal += "\n"
		}
	}
	
	var outputpath = "data/analysis.csv";
	fs.writeFile(outputpath, strfinal, function(err) {
		if(err) {
			return console.log(err);
		}

		console.log("The file was saved here: "+outputpath);
	}); 
	
	//objtofile(bench,"data/benchmark_analysis.json");
}



main();