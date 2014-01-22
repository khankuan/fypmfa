app.controller('DeviceAdminController', ['$scope', '$routeParams', '$location', 'devicesService', 
	function($scope, $routeParams, $location, devicesService) {
		$scope.address = $routeParams.deviceAddress;
    	$scope.devices = devicesService.getDevices();
		$scope.mfaDevice = new MFADevice($scope.devices[$scope.address]);
		$scope.status = 'connecting';

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

		$scope.backToDevices = function(){
			$location.path('/');
		}

		$scope.getInfo = function(){
			$scope.mfaDevice.getInfo(function(response){
				$scope.info = response;
				if (response == undefined)
					$scope.status = 'failed';
				else
					$scope.status = 'connected';
				$scope.safeApply();
			});
		}
		$scope.getInfo();

		$scope.updateInfo = function(password, newName, newPin, newPassword){
			if (password == undefined || password.length == 0)
				return;

			var info = {};
			if (newName != undefined && newName.length > 0)
				info.newName = newName;
			if (newPin != undefined && newPin.length > 0)
				info.newPin = newPin;
			if (newPassword != undefined && newPassword.length > 0)
				info.newPassword = newPassword;

			$scope.mfaDevice.updateInfo(password, info, function(response){
				if (response != undefined && response.success){
					setTimeout(function(){
						$scope.getInfo();
					},1000);
				}
			});
		}

		$scope.resetDevice = function(){
			$scope.mfaDevice.resetDevice(function(response){
				if (response != undefined && response.success){
					setTimeout(function(){
						$scope.getInfo();
					},1000);
				}
			});
		}

		$scope.getDomainSeed = function(pin, domain){
			var pinNonce = new Date().getTime();
			$scope.mfaDevice.getDomainSeed(domain, pinNonce, pin, function(response){
				$scope.requestedDomainSeed = response.domainSeed;
				$scope.safeApply();
			});
		}

		$scope.getDomainOTP = function(pin, domain){
			var pinNonce = new Date().getTime();
			$scope.mfaDevice.getDomainOTP(domain, pinNonce, pin, function(response){
				$scope.requestedDomainOTP = response.domainOTP;
				$scope.safeApply();
			});
		}

		$scope.$on("$destroy", function() {
        	$scope.mfaDevice.destroy();
        });

		$('body').hide();
		$('body').fadeIn();
	}
]);

