require File.dirname(__FILE__) + '/../../spec_helper'

context "Kernel.Array()" do
  specify "should call to_a to convert any arbitrary argument to an Array" do
    class KernelSpecArray
      def to_a; [:a, :b]; end
    end

    Array(KernelSpecArray.new).should == [:a, :b]
  end

  specify "should prefer to_ary over to_a" do
    class KernelSpecArray2
      def to_ary; [:a, :r, :y]; end
      def to_a; [:a, :b]; end
    end
    Array(KernelSpecArray2.new).should == [:a, :r, :y]
  end

  specify "should raise a TypeError if to_a doesn't return an Array" do
    class KernelSpecArray3
      def to_a; 'har'; end
    end

    should_raise(TypeError) { Array(KernelSpecArray3.new) }
  end

  specify "called with nil as argument should return an empty Array" do
    Array(nil).should == []
  end
end
