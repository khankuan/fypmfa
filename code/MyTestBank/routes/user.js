 /*

create?username=asd&password=test
auth?username=asd&password=test&domainOTPString=JSONString
logout?sessionId=absnfoaisnfoaisfasifsa

 */


module.exports = function(app) {
	var User = require('../models/User');
	var MFADevice = require('../models/MFADevice');
	var Session = require('../models/Session');
	var crypto = require('crypto');
	var MFA = require('mfa/mfa');
	var randomPasswordSeed = "asfnqiubfqifbqiwufbqiwufbnq12r1t1tg32g23";
	var randomSessionSeed = "n2u93ugn28yu9nivbuncoa--0ekfqw112r1r1";
 


	//	Create User
	app.get('/user/create',function(req,res){
		var username = req.query.username;
		var password = req.query.password;

		if (username == undefined || password == undefined || username.length == 0 || password.length == 0){
			res.status(400);
			res.send();
			return;
		}

		var hashedPassword = crypto.createHash('sha1').update(randomPasswordSeed+password).digest("hex");

		User.findOne({username: username}).exec(function(err, result) {
		  if (!err) {
		  	if (result != null){
		    	res.status(400);
				res.send({error: 'Username already exist.'});
			} else {
				var newUser = new User({
			  		username: username,
			  		hashedPassword: hashedPassword,
			  	});

			  	newUser.save(function(err){
			  		if (err){
			    		res.status(400);
			    		res.send();
			  		} else {
			    		res.status(200);
			    		res.send();
			  		}
			  	});
			}

		  } else {
		  	res.status(500);
		  	res.send();
		  };
		});
	})



	//	Auth User
	app.get('/user/auth',function(req,res){
		var username = req.query.username;
		var password = req.query.password;

		if (username == undefined || password == undefined || username.length == 0 || password.length == 0){
			res.status(400);
			res.send();
			return;
		}

		var hashedPassword = crypto.createHash('sha1').update(randomPasswordSeed+password).digest("hex");

		User.findOne({username: username}).exec(function(err, resultUser) {
		  if (!err && resultUser != null) {
		    //	Check hashedPassword
		    if (resultUser.hashedPassword != hashedPassword){
		    	res.status(401);
				res.send();
			}

		    //	Check MFA (to be done)
		    MFADevice.find({userId: resultUser._id}).exec(function(err, resultDevices) {
		    	var otps = {};
		    	for (var i in resultDevices)
		    		otps[resultDevices[i].uuid] = MFA.generateOTP(resultDevices[i].domainSeed);
		    	
		    	var match = 0;
				var domainOTPString = req.query.domainOTPString;
				if (domainOTPString){
					var domainOTPs = JSON.parse(domainOTPString);

					for (var i in domainOTPs){
						var domainOTP = domainOTPs[i];
						
						console.log(otps[domainOTP.uuid], domainOTP.domainOTP);
						if (otps[domainOTP.uuid] && otps[domainOTP.uuid] == domainOTP.domainOTP)
							match++;
					}
				}

				if (match < resultUser.deviceCount){
					res.status(400);
					res.send({reason: 'deviceCount insufficient'});
				}

			    //	Create session
			    var sessionId = crypto.createHash('sha1').update(randomSessionSeed+resultUser.id+new Date().getTime()).digest("hex");
			    var newSession = new Session({
			    	userId: resultUser._id,
			    	sessionId: sessionId
			    });
			    newSession.save(function(err){
			    	res.status(200);
					res.send({sessionId: sessionId, userId: resultUser.id, username: resultUser.username});
			    });
		    });


		  } else {
		    res.status(401);
			res.send();
		  };
		});
	});



	//	Logout User
	app.get('/user/logout',function(req,res){
		var sessionId = req.query.sessionId;

		if (sessionId == undefined || sessionId.length == 0){
			res.status(400);
			res.send();
			return;
		}

		Session.remove({sessionId: sessionId}).exec(function(err, result) {
		  if (!err && result == 1) {   
		    res.status(200);
			res.send();
		  } else {
		    res.status(400);
			res.send();
		  };
		});
	})
}