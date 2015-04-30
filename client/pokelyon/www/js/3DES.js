//TODO à définir
var key;
var iv;

/* alternatively, generate a password-based 16-byte key
var salt = forge.random.getBytesSync(128);
var key = forge.pkcs5.pbkdf2('password', salt, numIterations, 16);
*/

// encrypt some bytes using CBC mode
// (other modes include: CFB, OFB, CTR, and GCM)
function encrypt(input) {

  var cipher = forge.cipher.createCipher('3DES-CBC', key);
  cipher.start({iv: iv});
  cipher.update(input);
  cipher.finish();

  var output = forge.util.createBuffer();

  output.putBuffer(cipher.output);
  return output;
}

// decrypt some bytes using CBC mode
// (other modes include: CFB, OFB, CTR, and GCM)
function decrypt(input) {

  var decipher = forge.cipher.createDecipher('3DES-CBC', key);
  decipher.start({iv: iv});
  decipher.update(input);
  decipher.finish();
  var output = forge.util.createBuffer();

  output.putBuffer(decipher.output);
  return output;
}

