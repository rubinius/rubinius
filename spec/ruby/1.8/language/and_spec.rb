require File.dirname(__FILE__) + '/../spec_helper'

describe "The && keyword" do
  it "binds more tightly than =" do
    i = 1 && 10
    i.should == 10
  end
end

describe "The and keyword" do
  it "binds less tightly than =" do
    i = 1 and 10
    i.should == 1
  end
end
