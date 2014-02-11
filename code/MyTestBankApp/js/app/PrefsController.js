app.controller('PrefsController', ['$scope', '$location', '$rootScope', function($scope, $location, $rootScope) {
  	if (!$rootScope.sessionId || !$rootScope.userId || !$rootScope.username)
    	$location.path('/');

    $.ajax({
	  type: "GET",
	  url: "mytestbankapi/mfa/deviceCount",
	  data: { sessionId: $rootScope.sessionId }
	})
	  .done(function(msg) {
	  	$scope.deviceCount = msg.deviceCount;
	  	$scope.$apply();
	}).error(function(msg) {
	});

	$scope.refreshDevices = function(){
		$.ajax({
		  type: "GET",
		  url: "mytestbankapi/mfa/devices",
		  data: { sessionId: $rootScope.sessionId }
		})
		  .done(function(msg) {
		  	$scope.devices = msg.devices;
		  	$scope.$apply();
		}).error(function(msg) {
		});
	}
	$scope.refreshDevices();

    $scope.setDeviceCount = function(count){
    	$.ajax({
		  type: "GET",
		  url: "mytestbankapi/mfa/setDeviceCount",
		  data: { sessionId: $rootScope.sessionId, deviceCount: count }
		})
		  .done(function(msg) {
		  	$scope.deviceCount = count;
		  	$scope.$apply();
		}).error(function(msg) {
		});
    }

    $scope.removeDevice = function(uuid){
    	$.ajax({
		  type: "GET",
		  url: "mytestbankapi/mfa/delete",
		  data: { sessionId: $rootScope.sessionId, uuid: uuid }
		})
		  .done(function(msg) {
		  	delete $scope.devices[uuid];
		  	$scope.$apply();
		}).error(function(msg) {
		});
    }
	
    $scope.addDevices = function(domainSeedString){
    	$.ajax({
		  type: "GET",
		  url: "mytestbankapi/mfa/add",
		  data: { sessionId: $rootScope.sessionId, domainSeedString: domainSeedString }
		})
		  .done(function(msg) {
		  	$scope.refreshDevices();
		  	$scope.$apply();
		}).error(function(msg) {
		});
    }

    $scope.devicesEmpty = function(){
    	return Object.keys($scope.devices).length == 0;
    }

	$('body').hide();
	$('body').fadeIn();
}]);