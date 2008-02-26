require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#clone" do
  it "creates a new array containing all elements or the original" do
    a = [1, 2, 3, 4]
    b = a.clone
    b.should == a
    b.__id__.should_not == a.__id__
  end

  it "copies taint and frozen status from the original" do
    for taint in [ false, true ]
      for frozen in [ false, true ]
        a = [1, 2, 3, 4]
        a.taint  if taint
        a.freeze if frozen
        b = a.clone

        a.frozen?.should == b.frozen?
        a.tainted?.should == b.tainted?
      end
    end
  end
end
