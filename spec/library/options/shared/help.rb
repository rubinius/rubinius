require File.dirname(__FILE__) + '/../fixtures/string'
require 'options'

shared :lib_options_usage_help do |cmd|
  describe "Getting help from the option parser" do
    it "provides a ##{cmd} message constructed from the given header and options" do
      o = Options.new 'Some message here'
      o.option '-h --help Displays a help message'
      o.option '-f --foo Does nothing useful', :one

      s = "SomemessagehereOptions:-h--helpDisplaysahelpmessage-f--fooARGDoesnothinguseful"
      o.usage.squish.should == s
    end
  end
end
