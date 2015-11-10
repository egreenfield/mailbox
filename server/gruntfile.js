module.exports = function(grunt) {

  // Project configuration.
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    lambda_invoke: {
        default: {
            options: {
                file_name:"src/mailbox.js",
                event:"test/event.json"

            }
        }
    },
    lambda_package: {
        default: {
        }
    },
    lambda_deploy: {
        default: {
            options: {
                // Task-specific options go here.
                timeout: 15
            },
            arn:"arn:aws:lambda:us-west-2:669276666408:function:CheckMailbox"
        }
    },

  });

  // Load the plugin that provides the "uglify" task.
  grunt.loadNpmTasks('grunt-aws-lambda');

  // Default task(s).
//  grunt.registerTask('default', ['uglify']);

};