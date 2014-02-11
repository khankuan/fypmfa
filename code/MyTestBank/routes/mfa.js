 /*

deviceCount?sessionId=asfnqowifnq
setDeviceCount?sessionId=asfnqowifnq&deviceCount=2
add?sessionId=asfnqowifnq&uuid=2&domainSeedString=JSONString
delete?sessionId=asfnqowifnq&uuid=2

 */

module.exports = function(app) {
	var User = require('../models/User');
	var MFADevice = require('../models/MFADevice');
	var Session = require('../models/Session');
	var crypto = require('crypto');
 


	//	Set User device count
	app.get('/mfa/deviceCount',function(req,res){
		var sessionId = req.query.sessionId;

		if (sessionId == undefined || sessionId.length == 0){
			res.status(400);
			res.send();
			return;
		}

		Session.findOne({sessionId: sessionId}).exec(function(err, result) {
			if (err || result == null){
				res.status(400);
				res.send();
				return;
			}
			User.findOne({_id: result.userId}).exec(function(err, result) {
				if (err || result == null){
					res.status(400);
					res.send();
					return;
				}

				res.status(200);
				res.send({deviceCount: result.deviceCount});
			});
		});
	});



	//	set User device count
	app.get('/mfa/setDeviceCount',function(req,res){
		var sessionId = req.query.sessionId;
		var deviceCount = req.query.deviceCount;

		if (sessionId == undefined || deviceCount == undefined || sessionId.length == 0 || deviceCount.length == 0){
			res.status(400);
			res.send();
			return;
		}
		deviceCount = parseInt(deviceCount);

		Session.findOne({sessionId: sessionId}).exec(function(err, result) {
			if (result == null){
				res.status(400);
				res.send();
				return;
			}

			User.findOne({_id: result.userId}).exec(function(err, result) {
				if (err || result == null){
					res.status(400);
					res.send();
					return;
				}

				result.deviceCount = deviceCount;
				result.save(function(err){
					if (err){
						res.status(400);
						res.send();
					} else {
						res.status(200);
						res.send();
					}
				})
			});
		});
	});



	//	Add Device
	app.get('/mfa/add',function(req,res){
		var sessionId = req.query.sessionId;
		var domainSeedString = req.query.domainSeedString;

		if (sessionId == undefined || domainSeedString == undefined || sessionId.length == 0 || domainSeedString.length == 0){
			res.status(400);
			res.send();
			return;
		}

		var domainSeeds = JSON.parse(domainSeedString);
		Session.findOne({sessionId: sessionId}).exec(function(err, result) {
			if (err || result == null){
				res.status(400);
				res.send();
				return;
			}

			for (var i = 0; i < domainSeeds.length; i++){
				var cur = i;
				var uuid = domainSeeds[i].uuid;
				var name = domainSeeds[i].name;
				var domainSeed = domainSeeds[i].domainSeed;

				var newMFADevice = new MFADevice({
					userId: result.userId,
					uuid: uuid,
					name: name,
					domainSeed: domainSeed
				});
				newMFADevice.save(function(err){
					if (err && err.code != 11000){	//	duplicate
						console.log(err);
						res.status(400);
						res.send();
					}

					if (cur == domainSeeds.length-1){
						res.status(200);
						res.send();
					}
				});
			}
		});
	});



	//	Delete Device
	app.get('/mfa/delete',function(req,res){
		var sessionId = req.query.sessionId;
		var uuid = req.query.uuid;

		if (sessionId == undefined || uuid == undefined || sessionId.length == 0 || uuid.length == 0){
			res.status(400);
			res.send();
			return;
		}

		Session.findOne({sessionId: sessionId}).exec(function(err, result) {
			if (result == null){
				res.status(400);
				res.send();
				return;
			}

			MFADevice.remove({uuid: uuid}).exec(function(err, result) {
			  if (!err && result == 1) {   
			    res.status(200);
				res.send();
			  } else {
			    res.status(400);
				res.send();
			  };
			});
		});
	});
}