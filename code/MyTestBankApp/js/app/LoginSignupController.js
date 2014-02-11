app.controller('LoginSignupController', ['$scope', '$location', '$rootScope', function($scope, $location, $rootScope) {
  	if ($rootScope.sessionId && $rootScope.userId && $rootScope.username)
    	$location.path('home/');

	$scope.tabIndex = 0;

	$scope.login = function(){
		$.ajax({
		  type: "GET",
		  url: "mytestbankapi/user/auth",
		  data: { username: $scope.loginUsername, password: $scope.loginPassword, domainOTPString: $scope.loginOTPString }
		})
		  .done(function(msg) {
		  	$.cookie('username', msg.username);
		  	$.cookie('userId', msg.userId);
		  	$.cookie('sessionId', msg.sessionId);
		  	$rootScope.username = msg.username;
		  	$rootScope.userId = msg.userId;
		  	$rootScope.sessionId = msg.sessionId;
		  	$location.path('/home');
        	$rootScope.$apply();
		}).error(function(msg) {
			$scope.loginError = true;
			$scope.$apply();
		});
		$scope.loginPassword = '';
		$scope.loginOTPString = '';
	}


	$scope.signup = function(){
		$.ajax({
		  type: "GET",
		  url: "mytestbankapi/user/create",
		  data: { username: $scope.signupUsername, password: $scope.signupPassword }
		})
		  .done(function(msg) {
		  	$scope.loginUsername = $scope.signupUsername;
		  	$scope.loginPassword = $scope.signupPassword;
		  	$scope.login();
		}).error(function(msg) {
		  	$scope.signupError = true;
		  	$scope.$apply();
		});
	}
	
	$('body').hide();
	$('body').fadeIn();
}]);