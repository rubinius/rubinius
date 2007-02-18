require File.dirname(__FILE__) + '/../spec_helper'

context "IO class method" do
end

context "IO instance method" do
end

context "IO inherited instance method" do
  specify "instance_variable_get should return the value of the instance variable" do
    example do
      io = IO.new(1, "r")
      io.instance_variable_set(:@c, 1)
      io.instance_variable_get(:@c)
    end.should == 1
  end
  
  specify "instance_variable_get should return nil if the instance variable does not exist" do
    example do
      IO.new(1, "r").instance_variable_get(:@c)
    end.should == nil
  end
  
  specify "instance_variable_get should raise NameError if the argument is not of form '@x'" do
    example do
      try(NameError) { IO.new(1, "r").instance_variable_get(:c) }
    end.should == true
  end
end
