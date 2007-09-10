require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#instance_variable_get" do
  it "returns the value of the instance variable" do
    class Fred 
      def initialize(p1, p2) 
        @a, @b = p1, p2 
      end 
    end 
    fred = Fred.new('cat', 99) 
    fred.instance_variable_get(:@a).should == "cat"
    fred.instance_variable_get("@b").should == 99

    a = []
    a.instance_variable_set(:@c, 1)
    a.instance_variable_get(:@c).should == 1
  end

  it "returns nil if the instance variable does not exist" do
    [].instance_variable_get(:@c).should == nil
  end

  it "raises NameError exception if the argument is not of form '@x'" do
    class NoFred; end
    should_raise(NameError) { NoFred.new.instance_variable_get(:c) }
    should_raise(NameError) { [].instance_variable_get(:c) }
  end
end
