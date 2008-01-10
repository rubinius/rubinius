require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel.StringValue" do
  it "raises TypeError if the instance does not respond to to_str" do
    class A; end
    lambda { StringValue(A.new) }.should raise_error(TypeError)
  end

  it "raises TypeError for nil" do
    lambda { StringValue(nil) }.should raise_error(TypeError)
  end

  it "raises TypeError for a Fixnum" do
    lambda { StringValue(1) }.should raise_error(TypeError)
  end

  it "raises TypeError if the instance responds to to_str but doesn't return a String" do
    class B
      def to_str
        1
      end
    end
    lambda { StringValue(B.new) }.should raise_error(TypeError)
  end

  it "calls to_str if the instance responds to it" do
    class C
      def to_str
        "zam"
      end
    end
    StringValue(C.new).should == "zam"
  end
end
