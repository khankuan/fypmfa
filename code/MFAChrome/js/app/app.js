var app = angular.module('mfaApp',['ngRoute']);
 

app.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
      when('/', {
	    templateUrl: 'views/devices.html',
	    controller: 'DevicesController'
      }).
      when('/admin/:deviceAddress', {
	    templateUrl: 'views/deviceadmin.html',
	    controller: 'DeviceAdminController'
      });
}]);

