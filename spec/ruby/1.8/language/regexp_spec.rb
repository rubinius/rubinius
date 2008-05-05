require File.dirname(__FILE__) + '/../spec_helper'

describe "Literal Regexps" do
  it "matches against $_ (last input) in a conditional if no explicit matchee provided" do
    $_ = nil

    (true if /foo/).should_not == true

    $_ = "foo"

    (true if /foo/).should == true
  end
end
