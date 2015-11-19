var fs = require('fs');
var readline = require('readline');
var google = require('googleapis');
var googleAuth = require('google-auth-library');
var async = require('async');
var _ = require('underscore');
var gmail = google.gmail('v1');
var AWS = require('aws-sdk');
var spark = require('spark');


var config;
var accounts;
var googleAuthInstance;
var digest;
var response;
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
    loadProviders,
    loadAccounts,
    listUnreadEmail,
    filterEmail,
    extractTokens,
  ],function(err) {
    console.log("completed with err:",err);
    console.log("response",response);
    context.done(err,{digest:response});
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

function loadProviders(callback) {
  googleAuthInstance = new googleAuth();
  callback();  
}

/**
 * Create an OAuth2 client with the given credentials, and then execute the
 * given callback function.
 *
 * @param {Object} credentials The authorization client credentials.
 * @param {function} callback The callback to call with the authorized client.
 */
function loadAccounts(callback) {
  console.log("loading google provider");

  // Check if we have previously stored a token.
  fs.readFile(TOKEN_PATH, function(err, token) {
    if (err) return callback(null);
    config = JSON.parse(token);
    accounts = config.accounts;
    callback();
  });
}


function getMessageDetails(auth,message,callback) {
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
  digest = [];
  async.map(accounts,listGoogleEmail,function(err,messagesByAccount) {
    if(err) return callback(err);
    _.each(messagesByAccount,function(msgList) { digest = digest.concat(msgList);});
    return callback(err);
  });
}

function listGoogleEmail(account,callback) {
  console.log("listing for",account.nickname);
  var clientSecret = credentials.installed.client_secret;
  var clientId = credentials.installed.client_id;
  var redirectUrl = credentials.installed.redirect_uris[0];
  var results = [];

  var oauth2Client = new googleAuthInstance.OAuth2(clientId, clientSecret, redirectUrl);
  oauth2Client.credentials = account;

  gmail.users.messages.list({
    auth: oauth2Client,
    userId: 'me',
    q:"in:inbox is:unread -category:(promotions OR social)"
  }, function(err, response) {
    if (err) return callback(err);
    var messages = response.messages;
    if (messages == null || messages.length == 0) {
      console.log('No messages found.');
      return callback(null,results);
    } else {
      async.each(messages,function(m,cb) {
        getMessageDetails(oauth2Client,m,function(err,full) {
          results.push(full);
          cb();
        })
      },function(err) {
        return callback(err,results);
      });
    }
  });

}

function filterEmail(callback) {
  blacklist = config.blacklist;
  if(blacklist && blacklist.length) {
    blacklist = _.map(blacklist,function(exp) {return new RegExp(exp);});
    digest = _.filter(digest,function(element) {
      var blacklistMatch = _.find(blacklist,function(pattern) {
        return element.From.match(pattern);
      });
      return(!blacklistMatch);
    });    
  }
  whitelist = config.whitelist;
  if(whitelist && whitelist.length) {
    whitelist = _.map(whitelist,function(exp) {return new RegExp(exp);});
    digest = _.filter(digest,function(element) {
      var whitelistMatch = _.find(whitelist,function(pattern) {
        return element.From.match(pattern);
      });
      return(whitelistMatch);
    });    
  }
  callback();
}


function cleanTabs(str) {
  return str.replace("\t"," ");
}

function extractName(from) {
  return from.match(/(.*)\s+<.*>$/)[1];  
}

function extractTokens(callback) {
  console.log("updating flag");
  var tokenList = [];
  tokenList.push(digest.length);
//  response = "" + digest.length + "\t";
  if(digest.length) {
    _.each(digest,function(message) {
      tokenList.push(cleanTabs(message.Subject));
      tokenList.push(cleanTabs(extractName(message.From)));
      tokenList.push(new Date(message.Date).getTime());
    },response);
  }
  tokenList.push('\t\t\t');
  response = tokenList.join('\t');
  callback();
}