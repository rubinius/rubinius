# This file specifies behaviour for the methods of
# Continuation. The mechanics thereof may be further
# examined in spec/language.
#
require File.dirname(__FILE__) + '/../../spec_helper'

# Class methods
#   -
#
# Instance methods
#   #call             OK
#   #[]               OK


describe "Creating a Continuation object" do
  not_supported_on :jruby do
    it "must be done through Kernel.callcc, no .new" do
      lambda { Continuation.new }.should raise_error(NoMethodError)

      Kernel.callcc {|@cc|}
      c = @cc
      c.class.should == Continuation
    end
  end
end


describe "Executing a Continuation" do
  not_supported_on :jruby do
    it "using #call transfers execution to right after the Kernel.callcc block" do
      array = [:reached, :not_reached]

      Kernel.callcc {|@cc|}
    
      unless array.first == :not_reached
        array.shift
        @cc.call
      end

      array.should == [:not_reached]
    end

    it "arguments given to #call (or nil) are returned by the Kernel.callcc block (as Array unless only one object)" do
      Kernel.callcc {|cc| cc.call}.should == nil 
      Kernel.callcc {|cc| cc.call 1}.should == 1 
      Kernel.callcc {|cc| cc.call 1, 2, 3}.should == [1, 2, 3] 
    end

    it "#[] is an alias for #call" do
      Kernel.callcc {|cc| cc.call}.should == Kernel.callcc {|cc| cc[]}
      Kernel.callcc {|cc| cc.call 1}.should == Kernel.callcc {|cc| cc[1]}
      Kernel.callcc {|cc| cc.call 1, 2, 3}.should == Kernel.callcc {|cc| cc[1, 2, 3]} 
    end
  end
end
