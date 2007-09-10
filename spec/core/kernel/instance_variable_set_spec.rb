require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel#instance_variable_set" do
  it "sets the value of the specified instance variable" do
    class Dog
      def initialize(p1, p2) 
        @a, @b = p1, p2 
      end 
    end 
    Dog.new('cat', 99).instance_variable_set(:@a, 'dog').should == "dog"
  end

  it "sets the value of the instance variable when no instance variables exist yet" do
    class NoVariables; end
    NoVariables.new.instance_variable_set(:@a, "new").should == "new"
  end

  it "raises NameError exception if the argument is not of form '@x'" do
    class NoDog; end
    should_raise(NameError) { NoDog.new.instance_variable_set(:c, "cat") }
  end
end
