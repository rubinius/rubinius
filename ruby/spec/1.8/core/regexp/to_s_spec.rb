require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#to_s" do
  it "returns a string in (?xxx:yyy) notation" do
    /ab+c/ix.to_s.should == "(?ix-m:ab+c)"
    /xyz/.to_s.should == "(?-mix:xyz)"
    /jis/s.to_s.should == "(?-mix:jis)"
  end
end
