 
module.exports = function(app) {
	var User = require('../models/User');
	var MFADevice = require('../models/MFADevice');
	var Session = require('../models/Session');
	var crypto = require('crypto');
	var randomPasswordSeed = "asfnqiubfqifbqiwufbqiwufbnq12r1t1tg32g23";
 
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

		User.findOne({username: username}).exec(function(err, result) {
		  if (!err && result != null) {
		    //	Check hashedPassword
		    if (result.hashedPassword != hashedPassword){
		    	res.status(401);
				res.send();
			}

		    //	Check MFA (to be done)


		    //	Create session
		    res.status(200);
			res.send();


		  } else {
		    res.status(401);
			res.send();
		  };
		});
	})
}