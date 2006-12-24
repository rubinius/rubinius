$base_dir = File.dirname(File.expand_path(__FILE__)) + "/"
$rubinius_base = ENV["RUBINIUS_PATH"] || ($base_dir + "../..")

require $rubinius_base + "/shotgun-tests/helper.rb"

system("#{$rubinius_base}/bin/rcompile #{$base_dir}/foounit.rb")
system("#{$rubinius_base}/bin/rcompile #{$base_dir}/rubicon_testcase.rb")

def run_single_bfts_testclass filename
  include RubiniusHelper
    system("#{$rubinius_base}/bin/rcompile #{$base_dir}/foounit.rb")
    system("#{$rubinius_base}/bin/rcompile #{$base_dir}/rubicon_testcase.rb")
    code = "$:.unshift(\"#{$base_dir}/\")\n"
    code << "require \"foounit\"\n"
    code << IO.read($base_dir + "/../test_true_class.rb")
    puts rp(code)
end

Dir[$base_dir + "/../test_*.rb"].each {
  |filename|
  begin
    run_single_bfts_testclass filename
  rescue => e
    puts e.inspect
  end
}
