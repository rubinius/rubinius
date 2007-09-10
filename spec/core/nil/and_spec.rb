require File.dirname(__FILE__) + '/../../spec_helper'

describe "NilClass#&" do
  def nil_and(other)
    nil & other
  end
  
  it "returns false" do
    nil_and(nil).should == false
    nil_and(true).should == false
    nil_and(false).should == false
    nil_and("").should == false
    nil_and(Object.new).should == false
  end
end
