require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#^" do
  before :each do
    @set = Set[1, 2, 3, 4]
  end
  
  it "returns a new set containing elements not in both self and other" do
    set = @set ^ Set[3, 4, 5]
    set.should == Set[1, 2, 5]
  end
  
  it "accepts any enumerable as other" do
    set = @set ^ [3, 4]
    set.should == Set[1, 2]
  end
end
