$base_dir = File.dirname(File.expand_path(__FILE__))
require $base_dir + "/../../shotgun-tests/helper.rb"

def work_around_bug
  include RubiniusHelper
    system("#{$base_dir}/../../bin/rcompile #{$base_dir}/foounit.rb")
    system("#{$base_dir}/../../bin/rcompile #{$base_dir}/rubicon_testcase.rb")
    code = "$:.unshift(\"#{$base_dir}/\")\n"
    code << "require \"foounit\"\n"
    code << IO.read($base_dir + "/../test_true_class.rb")
    puts rp(code)
end

work_around_bug
