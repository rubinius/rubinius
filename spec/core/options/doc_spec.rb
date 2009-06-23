require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Options#doc" do
  before :each do
    @opt = Rubinius::Options.new "command"
  end

  it "adds text to be displayed with #to_s" do
    @opt.doc "Some message"
    @opt.doc "Another message"
    @opt.to_s.should == <<-EOD
command

Some message
Another message
EOD
  end
end
