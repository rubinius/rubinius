require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#collect!" do
  it "destructively performs #collect and returns self" do
    saved = set = Set[1, 2, 3, 4, 5]
    set.collect! { |x| x * 2 }.should == saved
    set.should == Set[2, 4, 6, 8, 10]
  end
end
