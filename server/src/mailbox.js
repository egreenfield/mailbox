var fs = require('fs');
var readline = require('readline');
var google = require('googleapis');
var googleAuth = require('google-auth-library');
var async = require('async');
var _ = require('underscore');
var gmail = google.gmail('v1');
var auth;
var AWS = require('aws-sdk');
var spark = require('spark');

var previousDigest;
var digest;
var credentials;

/**
 * Don't hard-code your credentials!
 * Export the following environment variables instead:
 *
 * export AWS_ACCESS_KEY_ID='AKID'
 * export AWS_SECRET_ACCESS_KEY='SECRET'
 */

// Set your region for future requests.


var SCOPES = ['https://www.googleapis.com/auth/gmail.readonly'];
// var TOKEN_DIR = (process.env.HOME || process.env.HOMEPATH ||
//     process.env.USERPROFILE) + '/.credentials/';
var CONFIG_DIR = process.cwd() + "/config/";
var TOKEN_PATH = CONFIG_DIR + 'SECRET_google_access_tokens.json';
var GOOGLE_SECRET_PATH = CONFIG_DIR + 'SECRET_google_client_tokens.json';


var BUCKET_NAME = "com.elyandpilar.mailbox";
var S3_KEY_NAME = "message-digest.json";

AWS.config.region = 'us-west-2';


var DEVICE_ID = "3b003d001547343433313338"
var DEVICE_ACCESS_TOKEN = "8a3fdd6091ca0e6d498e599556b2f4a71657beea";


exports.handler = function(event,context) {
  // Load client secrets from a local file.

  async.series([
    loadClientSecrets,
    loadGoogleTokens,
//    loadPreviousDigest,
    listUnreadEmail,
    updateFlag,
  ],function(err) {
      console.log("completed with err:",err);
//        context.done(err,JSON.stringify(digest));
        context.done(err,true);
  })
}

function loadClientSecrets(callback) {
  console.log("loading client secrets");
  fs.readFile(GOOGLE_SECRET_PATH, function(err, content) {
    if (err) return callback(err);
    credentials = JSON.parse(content);
    callback();
  });
}
/**
 * Create an OAuth2 client with the given credentials, and then execute the
 * given callback function.
 *
 * @param {Object} credentials The authorization client credentials.
 * @param {function} callback The callback to call with the authorized client.
 */
function loadGoogleTokens(callback) {
  console.log("loading tokens");
  var clientSecret = credentials.installed.client_secret;
  var clientId = credentials.installed.client_id;
  var redirectUrl = credentials.installed.redirect_uris[0];
  auth = new googleAuth();
  var oauth2Client = new auth.OAuth2(clientId, clientSecret, redirectUrl);

  // Check if we have previously stored a token.
  fs.readFile(TOKEN_PATH, function(err, token) {
    if (err) return callback(null);
    oauth2Client.credentials = JSON.parse(token);
    auth = oauth2Client;
    callback();
  });
}


function getMessageDetails(message,callback) {
  gmail.users.messages.get({
    auth:auth,
    userId:"me",
    id:message.id
  },function(err,fullMessage) {
    if(err) return callback(err);
    var headers = _.reduce(fullMessage.payload.headers,function(o,header) {
        o[header.name] = header.value;
        return o;
      },
      {}
    );
//    console.log("headers is",JSON.stringify(headers,null,'\t'));
    return callback(null,headers);
  });
}
/**
 * Lists the labels in the user's account.
 *
 * @param {google.auth.OAuth2} auth An authorized OAuth2 client.
 */
function listUnreadEmail(callback) {
  console.log("listing unread email");
  gmail.users.messages.list({
    auth: auth,
    userId: 'me',
    q:"in:inbox is:unread -category:(promotions OR social)"
  }, function(err, response) {
    if (err) return callback(err);
    var messages = response.messages;
    digest = [];
    if (messages == null || messages.length == 0) {
      console.log('No messages found.');
      return callback();
    } else {
      async.each(messages,function(m,cb) {
        getMessageDetails(m,function(err,full) {
          digest.push(full);
          cb();
        })
      },function(err) {
        return callback(err);
      });
    }
  });
}
function cleanTabs(str) {
  return str.replace("\t"," ");
}
function updateFlag(callback) {
  console.log("updating flag");
  var method;
  var params;


  spark.login({accessToken: DEVICE_ACCESS_TOKEN},function(err) {
    if(err) return callback(err);
    spark.getDevice(DEVICE_ID, function(err, device) {
      if(err) return callback(err);

      if(digest.length) {
        method = "setMessage";
        params = "" + digest.length + "\t";
        params = _.reduce(digest,function(params,message) {
          return params + message.Subject + "\t" + message.From + "\t";
        },params);
        var pStack = [];
        //console.log('splitting params:',params);
        while(params.length) {
          pStack.push(params.slice(0,60))
          params = params.slice(60);
        }
        //console.log("split into",pStack);
        device.callFunction("clearMsg","",function(err) {
          if(err) return callback(err);
          async.each(pStack,function(paramFragment,callback) {
            device.callFunction("appendMsg",paramFragment,function(err) {
              callback(err);
            });
          },function(err) {
            if(err) return callback(err);
            device.callFunction("endMsg","",function(err) {
              callback(err);
            });
          });
        });
      }
      else {
        device.callFunction("clearSignal","",function(err) {
          if(err) return callback(err);
          console.log("success with",method);
          callback();
        })
      }

    });
  });
}

function loadPreviousDigest(callback) {
  console.log("loading previous digest from S3");
  var params = {Bucket: BUCKET_NAME, Key: S3_KEY_NAME};

  var s3 = new AWS.S3();
  s3.getObject(params,function(err,data) {
    if(err) return callback(err);
    console.log("got bucket back",data);
    previousDigest = JSON.parse(data.body.toString());
    console.log("previous digest is:",JSON.stringify(previousDigest));
    callback();
  });

}
