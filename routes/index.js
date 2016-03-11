var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');

var Sound = require("../models/sound.js");

router.get('/', function(req,res){

return res.json("hello world");

})

router.get('/api/post/:soundvalue', function(req,res){

var soundReading = req.params.soundvalue;
var soundInt = parseInt(soundReading);
console.log(soundInt);

var soundObject = {
        soundLevel: soundInt
    };

console.log(soundObject);

var sound = new Sound(soundObject);

sound.save(function(err,data){
    // if err saving, respond back with error
    if (err){
      var error = {status:'ERROR', message: 'Error saving sound'};
      return res.json(error);
    }

    console.log('saved a new sound!');
    console.log(data);

    // now return the json data of the new animal
    var jsonData = {
      status: 'OK',
      sound: data
    }

    return res.json(jsonData);
  }) 
})

module.exports = router;