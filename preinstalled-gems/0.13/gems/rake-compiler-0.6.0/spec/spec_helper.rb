# add lib directory to the search path
libdir = File.expand_path(File.join(File.dirname(__FILE__), '..', 'lib'))
$LOAD_PATH.unshift(libdir) unless $LOAD_PATH.include?(libdir)

require 'rubygems'
require 'spec'

# Console redirection helper
require File.expand_path(File.join(File.dirname(__FILE__), 'support/capture_output_helper'))

Spec::Runner.configure do |config|
  include CaptureOutputHelper
end

# Rake::Task matcher helper
Spec::Matchers.define :have_defined do |task|
  match do |tasks|
    tasks.task_defined?(task)
  end
end
