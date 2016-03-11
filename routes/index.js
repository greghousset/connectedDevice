var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');

router.get('/', function(req,res){

return res.json("hello world");

})

router.get('/api/post/:soundvalue', function(req,res){

var soundReading = req.params.soundvalue;
console.log(soundReading);

return res.json(soundReading);

})

module.exports = router;