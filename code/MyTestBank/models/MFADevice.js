var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var mfaDeviceSchema = new mongoose.Schema({
	userId: ObjectId,
	uuid: String,
	name: String,
	domainSeed: String,
	dateCreated: { type: Date, default: Date.now }
})

var MFADevice = mongoose.model('MFADevice', mfaDeviceSchema);
module.exports = MFADevice;