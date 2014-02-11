var app = angular.module('myTestBankApp',['ngRoute']);
 

app.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
      when('/', {
	     templateUrl: 'views/loginsignup.html',
	     controller: 'LoginSignupController'
      }).
      when('/home', {
       templateUrl: 'views/home.html',
       controller: 'HomeController'
      }).
      when('/prefs', {
       templateUrl: 'views/prefs.html',
       controller: 'PrefsController'
      });
}]);


app.run(function($rootScope, $location){
  $rootScope.username = $.cookie('username');
  $rootScope.userId = $.cookie('userId');
  $rootScope.sessionId = $.cookie('sessionId');

  $rootScope.safeApply = function(fn) {
      var phase = this.$root.$$phase;
      if(phase == '$apply' || phase == '$digest') {
        if(fn && (typeof(fn) === 'function')) {
          fn();
        }
      } else {
        this.$apply(fn);
      }
    };

  $rootScope.goToHome = function(){
    $location.path('home/');
    $rootScope.safeApply();
  }

  $rootScope.goToPrefs = function(){
    $location.path('prefs/');
    $rootScope.safeApply();
  }

  $rootScope.logout = function(){
    $.ajax({
      type: "GET",
      url: "mytestbankapi/user/logout",
      data: { sessionId: $rootScope.sessionId }
    })
      .done(function(msg) {
        $.removeCookie('username');
        $.removeCookie('userId');
        $.removeCookie('sessionId');
        $rootScope.userId = undefined;
        $rootScope.username = undefined;
        $rootScope.sessionId = undefined;
        $location.path('/');
        $rootScope.safeApply();
    }).error(function(msg) {
        $.removeCookie('username');
        $.removeCookie('userId');
        $.removeCookie('sessionId');
        $rootScope.userId = undefined;
        $rootScope.username = undefined;
        $rootScope.sessionId = undefined;
        $location.path('/');
        $rootScope.safeApply();
    });
  }
  
});