require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#<=>" do
  it "returns -1, 0, 1 when self is less than, equal, or greater than other" do
    # 3 comparisions causes rubinius to crash, 2 comparisions work fine 
    # and each comparision tested singularly is ok
    (-3 <=> -1).should == -1
    (954 <=> 954).should == 0
    (496 <=> 5).should == 1
  end

  it "coerces fixnum and return -1, 0, 1 when self is less than, equal, or greater than other" do
    (-1 <=> 0xffffffff).should == -1
    (954 <=> 954.0).should == 0
    (496 <=> 5).should == 1
  end
end
