require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#callcc" do
  it "should be possible to exit a loop like a break" do
    i = 0
    Kernel.callcc do |x|
      loop do
        i += 1
        x.call() if i == 5
      end
    end.should == nil
    i.should == 5
  end

  it "should be possible to call a continuation multiple times" do
    i = 0
    cont = nil
    Kernel.callcc {|cont|}
    i += 1
    cont.call() if i < 5
    i.should == 5    
  end
  
  it "raises a LocalJumpError if callcc is not given a block" do
    should_raise(LocalJumpError) { Kernel.callcc }
  end
end
