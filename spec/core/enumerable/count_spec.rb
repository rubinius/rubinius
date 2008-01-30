require File.dirname(__FILE__) + '/../../spec_helper'

describe "Enumerable#count" do

  # Assuming array doesn't override count.  If this changes, use
  # EachDefiner from core/enumerable/fixtures/classes.rb

  it "requires an argument or a block" do
    lambda { [1, 2, 3, 4].count }.should raise_error
  end

  it "counts nils if given nil as an argument" do
    [nil, nil, nil, false].count(nil).should == 3
  end

  it "accepts an argument for comparison using ==" do
    ary = [1, 2, 4, 2]
    ary.count(2).should == 2
  end

  it "uses a block for comparison" do
    ary = [1, 2, 4, 2]
    ary.count{|x|x%2==0}.should == 3
  end

end