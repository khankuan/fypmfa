var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var userSchema = new mongoose.Schema({
	username: { type: String, unique: true },
	hashedPassword: String,
	dateCreated: { type: Date, default: Date.now },
	deviceCount: { type: Number, default: 0}
})

var User = mongoose.model('User', userSchema);
module.exports = User;