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

Sample queries:
{"queryType":"getDomainOTP_E_Pin","domain":"mail.google.com","pinNonce":1392606560266,"timestamp":1392606560267} 
{"queryType":"getInfo","timestamp":1392606608574}

*/



var MFADevice = function(device){

  var status = 'alive';
  this.log = true;
  this.timeout = 5000;
  var self = this;

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
   
  this.getDomainSeed_E_Pin = function(domain, pinNonce, callback){
    var q = {queryType: "getDomainSeed_E_Pin"};
    q.domain = domain;
    q.pinNonce = pinNonce;
    query(q, callback);
  }

  this.getDomainSeed = function(domain, pin, callback){
    var pinNonce = new Date().getTime();
    this.getDomainSeed_E_Pin(domain, pinNonce, function(response){
      if (response != undefined && response.domainSeed_E_Pin != undefined)
        response.domainSeed = xorBase64(response.domainSeed_E_Pin, hash(hash(pin) + pinNonce));
      callback(response);
    })
  }

  this.getDomainOTP_E_Pin = function(domain, pinNonce, callback){
    var q = {queryType: "getDomainOTP_E_Pin"};
    q.domain = domain;
    q.pinNonce = pinNonce;
    query(q, callback);
  }

  this.getDomainOTP = function(domain, pin, callback){
    var pinNonce = new Date().getTime();
    this.getDomainOTP_E_Pin(domain, pinNonce, function(response){
      if (response != undefined && response.domainOTP_E_Pin != undefined)
        response.domainOTP = xorBase64(response.domainOTP_E_Pin, hash(hash(pin) + pinNonce));
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
  var triesLimit = 22;

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
        if (s && s.device.address != device.address)
          return;

        if (s == undefined){
          delete callbacks[q.queryType+"_"+q.timestamp];
          responseCallback();
          busy = false;
          return;
        }

        if (self.log)
          console.log("Connected at socket", s);

        socket = s;
        readSocket();
        tries = 0;
        send(JSON.stringify(q));
      });

      clearTimeout(busyTimer);
      busyTimer = setTimeout(function(){
        busy = false;
      },3000);
    }
    doQuery();

    setTimeout(function(){
      if (self.log && callbacks[q.queryType+"_"+q.timestamp] != undefined)
        console.log("Timeout", q);

      if (callbacks[q.queryType+"_"+q.timestamp] != undefined)
        callbacks[q.queryType+"_"+q.timestamp]();
      delete callbacks[q.queryType+"_"+q.timestamp];
    }, self.timeout);
  }


  function connectCallback(socket){
    if (status == 'alive' && deviceConnectCallback)
      deviceConnectCallback(socket);
  }
  chrome.bluetooth.onConnection.addListener(connectCallback);

  function connect(callback){
    chrome.bluetooth.addProfile(profile, function(){
      deviceConnectCallback = callback;
      chrome.bluetooth.connect({device: device, profile: profile}, function(){
          if (chrome.runtime.lastError) {
            if (self.log)
              console.log("Connection fail", device, profile, chrome.runtime.lastError.message);
            connectCallback();
            busy = false;
            return;
          }
      });
    });
  }

  function disconnect(disconnectCallback){
    chrome.bluetooth.disconnect({socket:socket}, function(){
      if (self.log)
        console.log("Disconnected", device);
      if (disconnectCallback != undefined)
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
            return;
          }
        }
        readSocket();
      });
    }, 200);
  }

  function responseHandler(message){
    console.log("response", message);
    if (message.indexOf("\n") >= 0)
      message = message.substring(0, message.indexOf("\n"));

    disconnect(function(){
      var msg = JSON.parse(message);
      var callback = callbacks[msg.queryType+"_"+msg.timestamp];
      if (callback != undefined)
        callback(msg);
      delete callbacks[msg.queryType+"_"+msg.timestamp];
      busy = false;
    });
  }

  function send(msg){console.log(msg);
    chrome.bluetooth.write({socket:socket, data: str2ab(msg)}, function(bytes) {
      if (chrome.runtime.lastError) {
        setTimeout(function(){
          if (tries < triesLimit){
            send(msg);
          } else {
            //disconnect(function(){});
            //busy = false;
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
    var buf = new ArrayBuffer(str.length+1);
    var bufView = new Uint8Array(buf);
    for (var i = 0, strLen = str.length; i < strLen; i++)
      bufView[i] = str.charCodeAt(i);
    bufView[str.length] = delimiter;
    return buf;
  }

  function hash(msg){
    return CryptoJS.SHA1(msg).toString(CryptoJS.enc.Base64);
  }

  function xorBase64(msg, key){
    msg = atob(msg);
    key = atob(key);
    var output = "";
    for (var i = 0; i < msg.length; i++)
        output += String.fromCharCode(msg.charCodeAt(i) ^ key.charCodeAt(i % key.length));
    return btoa(output);
  }
}