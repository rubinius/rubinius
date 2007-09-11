require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#freeze" do
  it "prevents further modifications to self" do
    m = Module.new.freeze
    m.frozen?.should == true
    
    # Does not raise
    class << m; end
    
    should_raise(TypeError, "can't modify frozen object") do
      class << m
        def test() "test" end
      end
    end

    should_raise(TypeError, "can't modify frozen object") do
      def m.test() "test" end
    end
  end
end
