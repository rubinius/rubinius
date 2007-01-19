def miniunit?
  ENV['TEST_UNIT'] == 'mini'
end

harness = miniunit? ? 
    File.expand_path(File.dirname(__FILE__) + '/../native/test/unit') : 'test/unit'
begin
  require harness
rescue LoadError
  raise Exception, "Unable to require test harness: #{harness}"
end

# rubygems will prevent tests running on shotgun
require 'rubygems' unless miniunit?
