require File.expand_path('../../fixtures/string', __FILE__)
require 'options'

describe :lib_options_usage_help, :shared => true do
  it "provides a message constructed from the given header and options" do
    o = Options.new 'Some message here'
    o.option '-h --help Displays a help message'
    o.option '-f --foo Does nothing useful', :one

    s = "SomemessagehereOptions:-h--helpDisplaysahelpmessage-f--fooARGDoesnothinguseful"
    o.usage.squish.should == s
  end
end
