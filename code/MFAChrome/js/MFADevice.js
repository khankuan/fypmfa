/*
OpenMFADevice

getUuid: function (callback)
getName: function (callback)
setName: function (password, newName, callback)
setPassword: function (oldPassword, newPassword, callback)
setPin: function (password, newPin, callback)
getSeedDomain_E_Pin: function (domain, pinNonce, callback)
getOTP_E_Pin: function (domain, pinNonce, callback)
resetDevice: function (callback)
info: function (callback)
*/



var MFADevice = function(device){

  var status = 'alive';

  //  OpenMFA methods
  this.getUuid = function(callback){
    var q = {queryType: "getUuid"};
    query(q, callback);
  }

  this.getName = function(callback){
    var q = {queryType: "getName"};
    query(q, callback);
  }

  this.setPassword = function(password, newPassword, callback){
    var q = {queryType: "setPassword"};
    q.password = password;
    q.newPassword = newPassword;
    query(q, callback);
  }

  this.setPin = function(password, newPin, callback){
    var q = {queryType: "setPin"};
    q.password = password;
    q.newPin = newPin;
    query(q, callback);
  }

  this.setName = function(password, newName, callback){
    var q = {queryType: "setName"};
    q.password = password;
    q.newName = newName;
    query(q, callback);
  }
   
  this.getSeedDomain_E_Pin = function(domain, pinNonce, callback){
    var q = {queryType: "getSeedDomain_E_Pin"};
    q.domain = domain;
    q.pinNonce = pinNonce;
    query(q, callback);
  }

  this.getSeedDomain = function(domain, pinNonce, pin, callback){
    this.getSeedDomain_E_Pin(domain, pinNonce, function(response){
      if (response.seedDomain_E_Pin != undefined)
        response.seedDomain = xor(response.seedDomain_E_Pin, hash(hash(pin) + pinNonce));
      callback(response);
    })
  }

  this.getOTP_E_Pin = function(domain, pinNonce, callback){
    var q = {queryType: "getOTP_E_Pin"};
    q.domain = domain;
    q.pinNonce = pinNonce;
    query(q, callback);
  }

  this.getOTP = function(domain, pinNonce, pin, callback){
    this.getOTP_E_Pin(domain, pinNonce, function(response){
      if (response.otp_E_Pin != undefined)
        response.otp = xor(response.otp_E_Pin, hash(hash(pin) + pinNonce));
      callback(response);
    })
  }

  this.resetDevice = function(callback){
    var q = {queryType: "resetDevice"};
    query(q, callback);
  } 
  
  this.getInfo = function(callback){
    var q = {queryType: "getInfo"};
    query(q, callback);
  } 

  this.updateInfo = function(password, info, callback){
    var q = {queryType: "updateInfo"};
    q.password = password;
    if (info.newName != undefined)
      q.newName = info.newName;
    if (info.newPin != undefined)
      q.newPin = info.newPin;
    if (info.newPassword != undefined)
      q.newPassword = info.newPassword;
    query(q, callback);
  }
  
  this.destroy = function(){
    status = 'destroyed';
  };


  /**  Internal methods */

  //  Variables
  var callbacks = {};
  var profile = {"uuid": "00001101-0000-1000-8000-00805F9B34FB"};
  var busy = false;
  var socket;
  var delimiter = 10;
  var tries;
  var deviceConnectCallback;
  var busyTimer;
  var triesLimit = 25;

  //  Connection methods
  function isBusy(){
    return busy;
  }

  function query(q, responseCallback){
    if (busy)
      return;
    busy = true;
    function doQuery(){
      q.timestamp = new Date().getTime();
      callbacks[q.queryType+"_"+q.timestamp] = responseCallback;
      connect(function(s){
        if (s == undefined){
          responseCallback();
          busy = false;
          return;
        }
        socket = s;
        readSocket();
        tries = 0;
        send(JSON.stringify(q));
      });
      clearTimeout(busyTimer);
      busyTimer = setTimeout(function(){
        busy = false;
      },5000);
    }
    doQuery();
  }


  function connectCallback(socket){
    if (status == 'alive')
      deviceConnectCallback(socket);
  }
  chrome.bluetooth.onConnection.addListener(connectCallback);

  function connect(callback){
    chrome.bluetooth.addProfile(profile, function(){
      deviceConnectCallback = callback;
      chrome.bluetooth.connect({device: device, profile: profile}, function(){
          if (chrome.runtime.lastError) {
            console.log(chrome.runtime.lastError.message);
            connectCallback();
            busy = false;
            return;
          }
      });
    });
  }

  function disconnect(disconnectCallback){
    chrome.bluetooth.disconnect({socket:socket}, function(){
      disconnectCallback();
    });
  }


  var msg = "";
  function readSocket(){
    setTimeout(function(){
      chrome.bluetooth.read({socket:socket}, function(arraybuffer){
        var data = ab2str(arraybuffer);
        msg += data;
        for (var i = 0; i < data.length; i++){
          if (data.charCodeAt(i) == delimiter){
            responseHandler(msg.substring(0, msg.length - 1));
            msg = "";
            return;;
          }
        }
        readSocket();
      });
    }, 200);
  }

  function responseHandler(message){
    console.log("response", message);
    disconnect(function(){
      var msg = JSON.parse(message);
      var callback = callbacks[msg.queryType+"_"+msg.timestamp];
      if (callback != undefined)
        callback(msg);
      busy = false;
    });
  }

  function send(msg){
    chrome.bluetooth.write({socket:socket, data: str2ab(msg)}, function(bytes) {
      if (chrome.runtime.lastError) {
        setTimeout(function(){
          if (tries < triesLimit){
            send(msg);
          } else {
            disconnect(function(){
            });
            busy = false;
          }
          tries++;
        }, 200);
      }
    });
  }

  function ab2str(buf) {
    return String.fromCharCode.apply(null, new Uint8Array(buf));
  }

  function str2ab(str) {
    var buf = new ArrayBuffer(str.length+1); // 2 bytes for each char
    var bufView = new Uint8Array(buf);
    for (var i = 0, strLen = str.length; i < strLen; i++)
      bufView[i] = str.charCodeAt(i);
    bufView[str.length] = delimiter;
    return buf;
  }

  function hash(msg){
    return CryptoJS.SHA1(msg).toString(CryptoJS.enc.hex);
  }

  function xor(msg, key){
    var output = "";
    for (var i = 0; i < msg.length; i++)
        output += String.fromCharCode(msg.charCodeAt(i) ^ key.charCodeAt(i % key.length));
    return output;
  }
}