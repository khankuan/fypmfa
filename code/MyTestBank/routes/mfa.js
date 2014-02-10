 
module.exports = function(app) {
	var User = require('../models/User');
	var MFADevice = require('../models/MFADevice');
	var Session = require('../models/Session');
	var crypto = require('crypto');
 
	app.get('/mfa/deviceCount',function(req,res){
		
	});

	app.get('/mfa/add',function(req,res){
		
	});

	app.get('/mfa/remove',function(req,res){
		
	});
}