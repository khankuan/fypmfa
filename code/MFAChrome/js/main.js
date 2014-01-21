

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

chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('window.html', {
    'bounds': {
      'width': 800,
      'height': 600
    }
  });
});