

/** Functions to test */
function responsePrint(response){
  console.log(response);
}
/*
d.getUuid(responsePrint);
getName: function (callback)
setName: function (password, newName, callback)
setPassword: function (oldPassword, newPassword, callback)
setPin: function (password, newPin, callback)
getSeedDomain_E_Pin: function (domain, pinNonce, callback)
getOTP_E_Pin: function (domain, pinNonce, callback)
resetDevice: function (callback)
info: function (callback)
*/


/*	Admin interface	*/
chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('window.html', {
    'bounds': {
      'width': 800,
      'height': 600
    }
  });
});

/*	Message Handler	*/
chrome.runtime.onMessageExternal.addListener(function (message, sender, sendResponse){
	/*	DomainSeed, DomainOTP	*/
	if (message.type == "GetDomainSeed" || message.type == "GetDomainOTP"){

		var devices = {};
		chrome.bluetooth.getDevices({
		    deviceCallback: function(device) {
		    	devices[device.address] = device;
		    }
		}, function() {
		});

		setTimeout(function(){
			chrome.storage.local.get('devicesAddresses', function(result){
				if (result.devicesAddresses == undefined)
					sendResponse([]);

				var devicesAddresses = JSON.parse(result.devicesAddresses);
				var devicesKey = Object.keys(devicesAddresses);
				var devicesPIN = message.devicesPIN;

				var done = 0;
				var results = {};
				function itemCallback(address, result){
					done++;

					if (result != undefined)
						results[address] = result;

					console.log("done++", result);
					if (done == devicesKey.length){
						console.log("done", {type:message.type, results:results}, sender);
						var m = {};
						m.type = message.type;
						m.results = results;
						chrome.runtime.sendMessage(sender.id, m);
					}
				}

				function callAPI(i){
					if (i >= devicesKey.length)
						return;

					//	PIN not given
					if (devicesPIN[devicesKey[i]] == undefined){
						itemCallback();
						callAPI(i+1);
						return;
					}

					var device = new MFADevice(devices[devicesKey[i]]);
					console.log("Getting", devicesKey[i]);
					
					if (message.type == "GetDomainSeed"){
						device.getDomainSeed(message.domain, devicesPIN[devicesKey[i]], function(response){
							if (response != undefined)
								itemCallback(devicesKey[i], {domainSeed: response.domainSeed, uuid: response.uuid, name: response.name});
							else
								itemCallback();

							device.destroy();

							var delay = 1000;
							if (response == undefined)
								delay = 4000;

							setTimeout(function(){
								callAPI(i+1);
							},delay);
						});
					} else if (message.type == "GetDomainOTP"){
						device.getDomainOTP(message.domain, devicesPIN[devicesKey[i]], function(response){
							if (response != undefined)
								itemCallback(devicesKey[i], {domainOTP: response.domainOTP, uuid: response.uuid, name: response.name});
							else
								itemCallback();
							
							device.destroy();

							var delay = 1000;
							if (response == undefined)
								delay = 4000;

							setTimeout(function(){
								callAPI(i+1);
							},delay);
						});
					}
				}

				callAPI(0);
			});
		},1000);
	}


	if (message.type == "LaunchInterface"){
		chrome.app.window.create('window.html', {
		    'bounds': {
		      'width': 800,
		      'height': 600
		    }
		});
	}


	if (message.type == "GetDevices"){
		chrome.storage.local.get('devicesAddresses', function(result){
			var results = {};
			if (result.devicesAddresses != undefined)
				results = JSON.parse(result.devicesAddresses);
			console.log(result.devicesAddresses);
			chrome.runtime.sendMessage(sender.id, {type:message.type, results:results});
		});
	}
})