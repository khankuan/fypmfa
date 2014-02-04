app.controller('DevicesController', ['$scope', '$location', function($scope, $location) {
    $scope.addedDevicesAddresses = {};	//	To load from store
    $scope.includeDevices = {};	//	To load from store
    $scope.devicesPIN = {};
    $scope.tabDomain = "asd";

	$scope.safeApply = function(fn) {
		var phase = this.$root.$$phase;
		if(phase == '$apply' || phase == '$digest') {
			if(fn && (typeof(fn) === 'function')) {
		 		fn();
			}
		} else {
			this.$apply(fn);
		}
	};

	chrome.runtime.onMessageExternal.addListener(function (message, sender, sendResponse){
		if (message.type == "GetDevices"){
			$scope.addedDevicesAddresses = message.items;
			for (var d in message.items){
				if ($scope.devicesPIN[d] == undefined)
					$scope.devicesPIN[d] = "";
				if ($scope.includeDevices[d] == undefined)
					$scope.includeDevices[d] = false;
			}
			$scope.safeApply();
		}

		if (message.type == "GetDomainSeed"){
			var domainSeeds = [];
			for (var d in message.items)
				domainSeeds.push(message.items[d]);
			$scope.fetchedDomainSeedsCount = domainSeeds.length;
			console.log(message);
			copyTextToClipboard(JSON.stringify(domainSeeds));
			$("#domainSeedsNotice").fadeIn();
			$('#domainSeedsButton').prop('disabled', false);
			$('#domainSeedsButton').text('Fetch Domain Seeds (Setup)');
			setTimeout(function(){
				$("#domainSeedsNotice").fadeOut();
			},5000);
			$scope.safeApply();
		}

		if (message.type == "GetDomainOTP"){
			var OTPs = [];
			for (var d in message.items)
				OTPs.push(message.items[d]);
			$scope.fetchedDomainOTPsCount = OTPs.length;
			console.log(message);
			copyTextToClipboard(JSON.stringify(OTPs));
			$("#OTPsNotice").fadeIn();
			$('#OTPsButton').prop('disabled', false);
			$('#OTPsButton').text('Fetch Domain OTPs (Auth)');
			setTimeout(function(){
				$("#OTPsNotice").fadeOut();
			},5000);
			$scope.safeApply();
		}
	});

	$scope.init = function(){
		chrome.storage.local.get('popupData', function(result){
			var items = {};
			if (result.popupData != undefined){
				var popupData = JSON.parse(result.popupData);
				$scope.devicesPIN = popupData.devicesPIN;
				$scope.includeDevices = popupData.includeDevices;
				$scope.rememberPINs = popupData.rememberPINs;
				$scope.safeApply();
			}
			var m = {};
			m.type = "GetDevices";
		  	chrome.runtime.sendMessage("cinhnmhhlcpcdhpaanecmieiphgmddpg", m);
		});

		chrome.tabs.getSelected(function(tab){
			var tmp = document.createElement('a');
			tmp.href = tab.url;
			$scope.tabDomain = tmp.hostname;
			$scope.safeApply();
		});
	}
	$scope.init();

	$scope.launchAdmin = function(address){
		var m = {};
		m.type = "LaunchInterface";
	  	chrome.runtime.sendMessage("cinhnmhhlcpcdhpaanecmieiphgmddpg", m);
	}

	$scope.$watch('rememberPINs', function(){
		$scope.rememberPopupData();
	});


	$scope.rememberPopupData = function(){
		var d = {};
		d.devicesPIN = {};
		d.includeDevices = $scope.includeDevices;
		if ($scope.rememberPINs){
			d.devicesPIN = $scope.devicesPIN;
		}
		d.rememberPINs = $scope.rememberPINs;
		chrome.storage.local.set({'popupData': JSON.stringify(d)}, function(){
		});
	}

	$scope.getDomainSeed = function(){
		var m = {};
		m.type = "GetDomainSeed";
		m.devicesPIN = {};
		for (var d in $scope.includeDevices)
			if ($scope.includeDevices[d] == true)
				m.devicesPIN[d] = $scope.devicesPIN[d];
		m.domain = $scope.tabDomain;
		$scope.fetchingDomainSeedsCount = Object.keys(m.devicesPIN).length;
		chrome.runtime.sendMessage("cinhnmhhlcpcdhpaanecmieiphgmddpg", m);
		$scope.rememberPopupData();
		$('#domainSeedsButton').prop('disabled', true);
		$('#domainSeedsButton').text('Fetching Domain Seeds...');
	}

	$scope.getDomainOTP = function(){
		var m = {};
		m.type = "GetDomainOTP";
		m.devicesPIN = {};
		for (var d in $scope.includeDevices)
			if ($scope.includeDevices[d] == true)
				m.devicesPIN[d] = $scope.devicesPIN[d];
		m.domain = $scope.tabDomain;
		$scope.fetchingDomainOTPsCount = Object.keys(m.devicesPIN).length;
		chrome.runtime.sendMessage("cinhnmhhlcpcdhpaanecmieiphgmddpg", m);
		$scope.rememberPopupData();
		$('#OTPsButton').prop('disabled', true);
		$('#OTPsButton').text('Fetching Domain OTPs...');
	}

}]);
