require File.dirname(__FILE__) + '/../spec_helper'

# specs for __FILE__

describe "The __FILE__ constant" do
  it "should equal the current filename" do
    File.basename(__FILE__).should == "file_spec.rb"
  end

  it "should equal (eval) inside an eval" do
    eval("__FILE__").should == "(eval)"
  end
end
