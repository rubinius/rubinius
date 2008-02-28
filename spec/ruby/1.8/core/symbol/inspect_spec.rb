require File.dirname(__FILE__) + '/../../spec_helper'

describe "Symbol#inspect" do
  it "returns self as a symbol literal" do
    :fred.inspect.should   == ":fred"
    :fred?.inspect.should  == ":fred?"
    :fred!.inspect.should  == ":fred!"
    :[].inspect.should     == ":[]"
    :$-w.inspect.should    == ":$-w"
    :@ruby.inspect.should  == ":@ruby"
    :@@ruby.inspect.should == ":@@ruby"
  end
end
