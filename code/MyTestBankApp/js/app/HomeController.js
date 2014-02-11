app.controller('HomeController', ['$scope', '$location', '$rootScope', function($scope, $location, $rootScope) {
  	if (!$rootScope.sessionId || !$rootScope.userId || !$rootScope.username)
    	$location.path('/');

	$('body').hide();
	$('body').fadeIn();
}]);