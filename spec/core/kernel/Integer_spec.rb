require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.Integer" do
  it "should call to_i to convert any arbitrary argument to an Integer" do
    class KernelSpecInt
      def to_i; 7; end
    end

    Integer(KernelSpecInt.new).should == 7
  end

  it "should raise a TypeError if there is no to_i method on an object" do
    class KernelSpecInt2; end

    should_raise(TypeError) { Integer(KernelSpecInt2.new) }
  end

  it "should raise a TypeError if to_i doesn't return an Integer" do
    class KernelSpecInt3
      def to_i; 'har'; end
    end

    should_raise(TypeError) { Integer(KernelSpecInt3.new) }
  end
end
