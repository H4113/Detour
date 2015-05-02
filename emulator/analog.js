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
	var min = Infinity;
	var avg = 0.0;
	var count = 0;
	if(tab !== undefined)
	for(subtab of tab){
		for(val of subtab){
			if(val !== undefined && val.time != 0){
				if(val.time > max)
					max = val.time;
				if(val.time < min)
					min = val.time;
				avg += val.time;
				++count;
			}
		}
	}
	if(min == Infinity){
		min = 0;
	}
	if(count != 0)
		avg /= count;
	return {max:max,min:min,avg:avg};
}

function id(x,m){return x;}
function debit(num, max){
	num = 1.0/num;
	if(num == Infinity){
		num = max;
	}
	return num;
}

function main(){
	var options = {debit:false}
	process.argv.forEach(function (val, index, array) {
		if(index >= 2){
			if(val == "-d" || val =="--debit"){
				options.debit = true;
			}
		}
	});
	
	var f = id;
	if(options.debit){
		f = debit;
		console.log("Mode debit");
	}else{
		console.log("Mode response time");
	}
	
	var bench = JSON.parse(fs.readFileSync('data/benchmark.json', 'utf8'));
	strfinal = "nb,min,max,avg\n"
	for(var nbuser=0;nbuser<bench.length;++nbuser){
		if(bench[nbuser] !== null && bench[nbuser].length > 0){
			strfinal += nbuser +",";
			var info = getInfoTab(bench[nbuser]);
			strfinal += f(info.min,10)+",";
			strfinal += f(info.max,10)+",";
			strfinal += f(info.avg,10);
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
}

main();
