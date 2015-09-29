var express = require('express');
var Timeline = require('pebble-api');

// create a new Timeline object with our API key passed as an environment variable
var timeline = new Timeline();

var app = express(); // create an express instance
app.set('port', (process.env.PORT || 5000)); // set the port on the instance

// push a pin to :userToken from our webservice
app.get('/senduserpin/:userToken', function(req, res) {
  var userToken = req.params.userToken;
});

// start the webserver
var server = app.listen(app.get('port'), function () {
  console.log('Server listening on port %s', app.get('port'));
});