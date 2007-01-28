# This file specifies behaviour for the methods of
# Continuation. The mechanics thereof may be further
# examined in spec/language.
#
require File.dirname(__FILE__) + '/../spec_helper'

# Class methods
#   -
#
# Instance methods
#   #call             OK
#   #[]               OK


context 'Creating a Continuation object' do
  specify 'Must be done through Kernel.callcc, no .new' do
    example do
      n = Continuation.new rescue :failed

      Kernel.callcc {|@cc|}
      c = @cc

      [n, c.class]
    end.should == [:failed, Continuation]
  end
end


context 'Executing a Continuation' do
  specify 'Using #call transfers execution to right after the Kernel.callcc block' do
    example do
      array = [:reached, :not_reached]

      Kernel.callcc {|@cc|}
      
      unless array.first == :not_reached
        array.shift
        @cc.call
      end

      array
    end.should == [:not_reached]
  end

  specify 'Arguments given to #call (or nil) are returned by the Kernel.callcc block (as Array unless only one object)' do
    example do
      [Kernel.callcc {|cc| cc.call}, 
       Kernel.callcc {|cc| cc.call 1}, 
       Kernel.callcc {|cc| cc.call 1, 2, 3}] 
    end.should == [nil, 1, [1, 2, 3]]
  end

  specify '#[] is an alias for #call' do
    example do
      [Kernel.callcc {|cc| cc.call} == Kernel.callcc {|cc| cc[]},
       Kernel.callcc {|cc| cc.call 1} == Kernel.callcc {|cc| cc[1]},
       Kernel.callcc {|cc| cc.call 1, 2, 3} == Kernel.callcc {|cc| cc[1, 2, 3]}] 
    end.should == [true, true, true]
  end
end
