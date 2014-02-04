var app = angular.module('mfaApp',['ngRoute']);
 

app.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
      when('/', {
	    templateUrl: 'views/devices.html',
	    controller: 'DevicesController'
      });
}]);

function copyTextToClipboard(text) {
    var copyFrom = $('<textarea/>');
    copyFrom.text(text);
    $('body').append(copyFrom);
    copyFrom.select();
    document.execCommand('copy');
    copyFrom.remove();
}