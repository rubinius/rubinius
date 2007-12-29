require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module.new" do
  it "creates a new anonymous Module" do
    Module.new.is_a?(Module).should == true
  end
  
  fails_on :rbx do
    it "evaluates a passed block in the context of the module" do
      fred = Module.new do
        def hello() "hello" end
        def bye()   "bye"   end
      end
      
      (o = mock('x')).extend(fred)
      o.hello.should == "hello"
      o.bye.should   == "bye"
    end
  end
end
