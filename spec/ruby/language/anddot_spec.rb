require File.expand_path('../../spec_helper', __FILE__)

describe "Invoking a method with &." do
  it "invokes the method when the receiver is not nil" do
    a = Class.new do
      def m() :b end
    end.new

    a&.m.should == :b
  end

  it "does not invoke the method when the receiver is nil" do
    a = nil
    a&.m.should be_nil
  end
end

describe "Attribute assignment with &." do
  it "assigns an attribute when the receiver is not nil" do
    a = Class.new do
      attr_accessor :m
    end.new

    a&.m = 42
    a.m.should == 42
  end

  it "does not assign an attribute when the receiver is nil" do
    a = nil
    (a&.m = 42).should be_nil
  end
end

describe "Operator assignment with &." do
  it "assigns an attribute when the receiver is not nil" do
    a = Class.new do
      attr_accessor :m
    end.new

    a.m = 5
    (a&.m += 5).should == 10
  end

  it "assigns an attribute from a conditional operator expression when the receiver is not nil" do
    a = Class.new do
      attr_accessor :m
    end.new

    a.m = false
    (a&.m ||= true).should be_true
  end

  it "does not assign an attribute when the receiver is nil" do
    a = nil
    (a&.m += 5).should be_nil
  end

  it "does not assign an attribute from a conditional operator expression when the receiver is not nil" do
    a = nil
    (a&.m ||= true).should be_nil
  end
end

