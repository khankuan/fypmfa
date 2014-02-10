
//	Dependencies
var http = require('http');
var express = require('express');


//	Setup servers
var app = express();
app.set('port', process.env.PORT || 3000);
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.json());
app.use(express.urlencoded());
app.use(express.methodOverride());
app.use(app.router);

//	Routes
require('./routes/user')(app);
require('./routes/mfa')(app);


//	Mongod
var mongooseURL = "mongodb://127.0.0.1/mytestbank";
var mongoose = require('mongoose');
mongoose.connect(mongooseURL);


//	Start
http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});