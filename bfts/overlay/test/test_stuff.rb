$base_dir = File.dirname(File.expand_path(__FILE__))
require $base_dir + "/../../../shotgun-tests/helper.rb"

def work_around_bug
  include RubiniusHelper
    system("#{$base_dir}/../../../bin/rcompile #{$base_dir}/../../../native/test/foounit.rb")
    system("#{$base_dir}/../../../bin/rcompile #{$base_dir}/../rubicon_testcase.rb")
    miniunit_rb = $base_dir + "/../../../native/test/foounit"
    code = "require \"#{miniunit_rb}\"\n"
    code << "$:.unshift(\"#{$base_dir}/../\")\n"
    code << IO.read($base_dir + "/../../test_true_class.rb")
    puts rp(code)
end

work_around_bug