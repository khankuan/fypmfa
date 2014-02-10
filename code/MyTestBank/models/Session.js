var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var sessionSchema = new mongoose.Schema({
	userId: ObjectId,
	sessionId: String,
	dateCreated: { type: Date, default: Date.now }
})

var Session = mongoose.model('Session', sessionSchema);
module.exports = Session;