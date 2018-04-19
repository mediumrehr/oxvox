var bmp = require("bmp-js"),
	fs    = require("fs");

var filename = process.argv.slice(2)[0];
if (!filename || (filename.split('.')[1] != "bmp")) {
	console.log('use format: node bmp2bytes.js filename.bmp');
	return;
}

var bmpBuffer = fs.readFileSync(filename);
var bmpData = bmp.decode(bmpBuffer);
var pixelData = [];

// reduce to one channel and convert 255 to 1
for (var i = 0; i < bmpData.data.length; i+=4) {
	if (bmpData.data[i] == 255) {
		pixelData.push(1);
	} else if (bmpData.data[i] == 0) {
		pixelData.push(bmpData.data[i]);
	} else {
		console.log("unexpected value: " + bmpData.data[i]);
	}
}

// reorganize bits for ssd1306
var pixelBytes = [];
for (var i = 0; i < 8; i++) {
	for (var j = 0; j < 128; j++) {
		var myByte = 0;
		for (var k = 0; k < 8; k++) {
			myByte |= pixelData[i*1024 + j + k*128] << k;
		}
		pixelBytes.push(myByte);
	}
}

// convert values to hex, pad, and structure for easier array reading
var pixelBytesForArray = "";
for (var i = 0; i < pixelBytes.length; i+=16) {
	var byteLine = "";
	for (var j = 0; j < 16; j++) {
		var s = (pixelBytes[i+j]).toString(16);
	    if(s.length < 2) {
	        s = '0' + s;
	    }
		byteLine += "0x" + s + ", ";
	}
	byteLine += "\n";
	pixelBytesForArray += byteLine;
}

fs.writeFile(filename + "_bytes.txt", pixelBytesForArray, function(err) {
    if(err) {
        return console.log(err);
    }
    console.log("Saved to " + filename + "_bytes.txt!");
}); 