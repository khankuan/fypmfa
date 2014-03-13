app.controller('DevicesController', ['$scope', '$location', 'devicesService', function($scope, $location, devicesService) {
    $scope.localDevice;
    $scope.devices = devicesService.getDevices();
    $scope.addedDevicesAddresses = {};	//	To load from store

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

	$scope.init = function(){
		chrome.bluetooth.getAdapterState(function(result){
			console.log(result);
			$scope.localDevice = result;
		    $scope.safeApply();
		});

		chrome.bluetooth.getDevices(function(devices) {
			console.log(devices);
	      	for (var i in devices)
	    		$scope.devices[devices[i].address] = devices[i];
	        $scope.safeApply();
		});
	}
	$scope.init();


	$scope.addDeviceAddress = function(address, name){
		$scope.addedDevicesAddresses[address] = name;
		$scope.saveAddedDevicesToStore();
	}

	$scope.removeDeviceAddress = function(address){
		delete $scope.addedDevicesAddresses[address];
		$scope.saveAddedDevicesToStore();
	}

	$scope.saveAddedDevicesToStore = function(){
		chrome.storage.local.set({'devicesAddresses': JSON.stringify($scope.addedDevicesAddresses)}, function(){
		});
	}

	$scope.loadAddedDevicesFromStore = function(){
		chrome.storage.local.get('devicesAddresses', function(result){
			if (result.devicesAddresses != undefined)
				$scope.addedDevicesAddresses = JSON.parse(result.devicesAddresses);
			$scope.safeApply();
		});
	}
	$scope.loadAddedDevicesFromStore();

	$scope.goToDeviceAdmin = function(address){
	  	$location.path('admin/'+address);
	}

	$('body').hide();
	$('body').fadeIn();
}]);
