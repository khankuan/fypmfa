var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var userSchema = new mongoose.Schema({
	username: String,
	hashedPassword: String,
	dateCreated: { type: Date, default: Date.now }
})

var User = mongoose.model('User', userSchema);
module.exports = User;