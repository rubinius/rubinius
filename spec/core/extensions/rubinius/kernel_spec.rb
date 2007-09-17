require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Kernel.StringValue" do
    specify "raises TypeError if the instance does not respond to to_str" do
      class A; end
      should_raise(TypeError) { StringValue(A.new) }
    end

    specify "raises TypeError for nil" do
      should_raise(TypeError) { StringValue(nil) }
    end

    specify "raises TypeError for a Fixnum" do
      should_raise(TypeError) { StringValue(1) }
    end

    specify "raises TypeError if the instance responds to to_str but doesn't return a String" do
      class B
        def to_str
          1
        end
      end
      should_raise(TypeError) { StringValue(B.new) }
    end

    specify "calls to_str if the instance responds to it" do
      class C
        def to_str
          "zam"
        end
      end
      StringValue(C.new).should == "zam"
    end
  end
end
