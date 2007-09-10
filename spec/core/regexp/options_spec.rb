require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#options" do
  it "returns set of bits for options used" do
    /cat/.options.should == 0
    /cat/ix.options.should == 3
    Regexp.new('cat', true).options.should == 1
    Regexp.new('cat', 0, 's').options.should == 48
  end
end
