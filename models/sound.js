var mongoose = require('mongoose');
var Schema = mongoose.Schema;

// See http://mongoosejs.com/docs/schematypes.html

var soundSchema = new Schema({
	soundLevel: Number
})

// export 'Animal' model so we can interact with it in other files
module.exports = mongoose.model('Sound',soundSchema);