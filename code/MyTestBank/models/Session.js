var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var sessionSchema = new mongoose.Schema({
	userId: ObjectId,
	sessionId: { type: String, unique: true },
	dateCreated: { type: Date, default: Date.now }
})

var Session = mongoose.model('Session', sessionSchema);
module.exports = Session;