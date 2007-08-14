require "test/unit"
dir = File.dirname(File.expand_path(__FILE__))
Dir["#{dir}/../test_*"].each do |f|
   require f 
end
