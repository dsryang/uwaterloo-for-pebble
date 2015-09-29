var UW_API_KEY = 'API_KEY';
var UW_ROOT_URL = 'https://api.uwaterloo.ca/v2';

var TIMELINE_ROOT_URL = 'https://timeline-api.getpebble.com/';

var INFO_SESSION = 10;
var INFO_SESSION_LIMIT = 15;
var MS_PER_MIN = 60000;

var termName;
var monthNames = ['Jan.', 'Feb.', 'Mar.', 'Apr.', 'May', 'June', 'July', 'Aug.', 'Sept.', 'Oct.', 'Nov.', 'Dec.']

var infoSessions = {
  date: [],
  details: {
    id: [],
    employer: [],
    date: [],
    time: [],
    duration: [],
    location: [],
    body: []
  }
};
var infoSessionPin = {
  'id': '',
  'time': '',
  'duration': 120,
  'layout': {
    'type': 'calendarPin',
    'title': "Info Session",
    "tinyIcon": "system://images/SCHEDULED_EVENT",
    'locationName': 'Unknown Location',
    'body': 'No description.'
  },
  'reminders': [
    {
      'time': '',
      'layout': {
        'type': 'genericReminder',
        'tinyIcon': 'system://images/SCHEDULED_EVENT',
        'title': 'Info Session',
        'locationName': ''
      }
    }
  ]
};

var currentDate = new Date();

function getTerm () {
  var WINTER_TERM = [1, 2, 3, 4];
  var SPRING_TERM = [5, 6, 7, 8];

  var currentMonth = currentDate.getMonth() + 1;
  var currentYear = currentDate.getFullYear();

  var currentTerm;

  if (WINTER_TERM.indexOf(currentMonth) >= 0) {
    currentTerm = '1' + currentYear.toString().substring(2, 4) + '1';
    termName = 'Winter ' + currentYear;
  }
  else if (SPRING_TERM.indexOf(currentMonth) >=0 ) {
    currentTerm = '1' + currentYear.toString().substring(2, 4) + '5';
    termName = 'Spring ' + currentYear;
  }
  else {
    currentTerm = '1' + currentYear.toString().substring(2, 4) + '9';
    termName = 'Fall ' + currentYear;
  }

  return currentTerm;
}

function sortInfoSessions (amount) {
  var swapped = 1;

  while (swapped != 0) {
    swapped = 0;

    for (var i = 0; i < amount - 1; i++) {
      if (infoSessions.date[i] > infoSessions.date[i + 1]) {
        // Info Sessions need to be swapped
        var temp = infoSessions.date[i];
        infoSessions.date[i] = infoSessions.date[i + 1];
        infoSessions.date[i + 1] = temp;

        temp = infoSessions.details.id[i];
        infoSessions.details.id[i] = infoSessions.details.id[i + 1];
        infoSessions.details.id[i + 1] = temp;
        temp = infoSessions.details.employer[i];
        infoSessions.details.employer[i] = infoSessions.details.employer[i + 1];
        infoSessions.details.employer[i + 1] = temp;
        temp = infoSessions.details.date[i];
        infoSessions.details.date[i] = infoSessions.details.date[i + 1];
        infoSessions.details.date[i + 1] = temp;
        temp = infoSessions.details.time[i];
        infoSessions.details.time[i] = infoSessions.details.time[i + 1];
        infoSessions.details.time[i + 1] = temp;
        temp = infoSessions.details.duration[i];
        infoSessions.details.duration[i] = infoSessions.details.duration[i + 1];
        infoSessions.details.duration[i + 1] = temp;
        temp = infoSessions.details.location[i];
        infoSessions.details.location[i] = infoSessions.details.location[i + 1];
        infoSessions.details.location[i + 1] = temp;
        temp = infoSessions.details.body[i];
        infoSessions.details.body[i] = infoSessions.details.body[i + 1];
        infoSessions.details.body[i + 1] = temp;

        swapped = 1;
      }
    }
  }
}

function getInfoSessionData () {
  var req = new XMLHttpRequest();
  var URL = UW_ROOT_URL + '/terms/' + getTerm() + '/infosessions.json?key=' + UW_API_KEY;
  req.open('GET', URL);
  //console.log("Opening URL: " + URL);

  req.onload = function() {
    if (req.readyState == 4 && req.status == 200) {
      var received = JSON.parse(req.responseText);
      if (received.meta.message == 'Request successful') {
        var infoSessionsData = received.data;
        console.log('Request successful');
        
        var sessionDate;
        var count = 0;

        infoSessions.date = [];
        infoSessions.details.id = [];
        infoSessions.details.employer = [];
        infoSessions.details.date = [];
        infoSessions.details.time = [];
        infoSessions.details.duration = [];
        infoSessions.details.location = [];
        infoSessions.details.body = [];
        
        for (var i = 0; i < infoSessionsData.length; i++) {
          sessionDate = new Date(infoSessionsData[i].date + ' ' + infoSessionsData[i].start_time);

          // Ensure info session exists (not a cancelled, rescheduled, not yet confirmed, or non-info session event) and is upcoming
          if (count < INFO_SESSION_LIMIT && infoSessionsData[i].location && infoSessionsData[i].employer.indexOf('*') == -1 && sessionDate.getTime() >= currentDate.getTime()) {
            var endDate = new Date(infoSessionsData[i].date + ' ' + infoSessionsData[i].end_time);
            var duration = (endDate.getTime() - sessionDate.getTime()) / (1000 * 60);
            var month = sessionDate.getMonth();
            var day = sessionDate.getDate();

            infoSessions.date.push(sessionDate);

            infoSessions.details.id.push(infoSessionsData[i].id);
            infoSessions.details.employer.push(infoSessionsData[i].employer);
            infoSessions.details.date.push(infoSessionsData[i].start_time + ' on ' + monthNames[month] + ' ' + day);
            infoSessions.details.time.push(sessionDate.toISOString());
            infoSessions.details.duration.push(duration);
            infoSessions.details.location.push(infoSessionsData[i].location);
            infoSessions.details.body.push(infoSessionsData[i].description);
            
            count++;
          }
          else if (count >= INFO_SESSION_LIMIT) {
            break;
          }
        }

        sendInfoSessionData();
      }
      else {
        console.log('Error: request unsuccessful');
      }
    }
    else {
      console.log('Unable to load with readyState: ' + req.readyState + ', status: ' + req.status);
    }
  };

  req.send(null);
}

function sendInfoSessionData () {
  var employers = '';
  var employers_index = [0];
  var dates = '';
  var dates_index = [0];
  var amount = infoSessions.date.length;
  //console.log('amount: ' + amount);

  sortInfoSessions(amount);

  for (var i = 0; i < amount; i++) {
    var length = infoSessions.details.employer[i].length;

    if (length > 20) {
      //console.log('employer length > 20');
      employers += infoSessions.details.employer[i].substring(0, 21);
      employers_index.push(21);
    }
    else {
      //console.log('employer length < 20');
      employers += infoSessions.details.employer[i] + ' ';
      employers_index.push(length + 1);
    }

    length = infoSessions.details.date[i].length;

    if (length > 20) {
      //console.log('date length > 20');
      dates += infoSessions.details.date[i].substring(0, 21);
      dates_index.push(21);
    }
    else {
      //console.log('date length < 20');
      dates += infoSessions.details.date[i] + ' ';
      dates_index.push(length + 1);
    }
  }

  //console.log('employers: ' + employers);
  //console.log('employers_index: ' + employers_index);
  //console.log('dates: ' + dates);
  //console.log('dates_index: ' + dates_index);

  Pebble.sendAppMessage({
    'KEY_RECEIVED_DATA': INFO_SESSION,
    'IS_EMPLOYER': employers,
    'IS_EMPLOYER_INDEX': employers_index,
    'IS_DATE': dates,
    'IS_DATE_INDEX': dates_index,
    'IS_AMOUNT': amount
  });
}

function pushPin (selected) {
  infoSessionPin.id = 'uw-info-session-pin-' + infoSessions.details.id[selected];
  infoSessionPin.time = infoSessions.details.time[selected];
  infoSessionPin.duration = infoSessions.details.duration[selected];
  infoSessionPin.layout.title = infoSessions.details.employer[selected] + ' Info Session';
  infoSessionPin.layout.locationName = infoSessions.details.location[selected];
  if (infoSessions.details.body[selected]) {
    infoSessionPin.layout.body = infoSessions.details.body[selected];
  }

  var reminderDate = new Date(infoSessions.date[selected] - (15 * MS_PER_MIN));
  //console.log('reminderDate ' + reminderDate);
  infoSessionPin.reminders[0].time = reminderDate.toISOString();
  infoSessionPin.reminders[0].layout.title = infoSessions.details.employer[selected] + ' Info Session';
  infoSessionPin.reminders[0].layout.locationName = infoSessions.details.location[selected];
  
  var req = new XMLHttpRequest();
  var URL = TIMELINE_ROOT_URL + 'v1/user/pins/' + infoSessionPin.id;
  req.onload = function () {
        console.log('timeline: response received: ' + this.responseText);
  };
  req.open('PUT', URL);
  
  Pebble.getTimelineToken (
    function (token) {
      console.log('My timeline token is ' + token);
      
      req.setRequestHeader('Content-Type', 'application/json');
      req.setRequestHeader('X-User-Token', token.toString());
      
      req.send(JSON.stringify(infoSessionPin));
      
      console.log('Sent pin: ' + JSON.stringify(infoSessionPin));
    },
    function (error) { 
      console.log('Error getting timeline token: ' + error);
    }
  );
}

Pebble.addEventListener('ready', function () {
  console.log("JS ready!");
});

Pebble.addEventListener('appmessage', function (e) {
  if (e.payload.KEY_GET_DATA === 10) {
    console.log('Received Get Data - Info Session Code');
    getInfoSessionData();
  }
  else if (e.payload.KEY_ADD_PIN === 10) {
    console.log('Received Add Pin - Info Session Code');
    console.log('selected: ' + e.payload.KEY_PIN_DATA);
    pushPin(e.payload.KEY_PIN_DATA);
  }
  else if (e.payload.KEY_SUBSCRIBE_TOPIC === 20) {
    console.log('Received Subscribe Topic - Lunch Menu Code');
    Pebble.timelineSubscribe('uwaterloo_lunch_menu', 
      function () { 
        console.log('Subscribed to Lunch Menu');
      },
      function (error) { 
        console.log('Error subscribing to Lunch Menu: ' + error);
      }
    );
  }
  else if (e.payload.KEY_UNSUBSCRIBE_TOPIC === 20) {
    console.log('Received Unsubscribe Topic - Lunch Menu Code');
    Pebble.timelineUnsubscribe('uwaterloo_lunch_menu', 
      function () { 
        console.log('Unsubscribed to Lunch Menu');
      },
      function (error) { 
        console.log('Error unsubscribing to Lunch Menu: ' + error);
      }
    );
  }
  else if (e.payload.KEY_SUBSCRIBE_TOPIC === 30) {
    console.log('Received Subscribe Topic - Dinner Menu Code');
    Pebble.timelineSubscribe('uwaterloo_dinner_menu', 
      function () { 
        console.log('Subscribed to Dinner Menu');
      },
      function (error) { 
        console.log('Error subscribing to Dinner Menu: ' + error);
      }
    );
  }
  else if (e.payload.KEY_UNSUBSCRIBE_TOPIC === 30) {
    console.log('Received Unsubscribe Topic - Dinner Menu Code');
    Pebble.timelineUnsubscribe('uwaterloo_dinner_menu', 
      function () { 
        console.log('Unsubscribed to Dinner Menu');
      },
      function (error) { 
        console.log('Error unsubscribing to Dinner Menu: ' + error);
      }
    );
  }
});