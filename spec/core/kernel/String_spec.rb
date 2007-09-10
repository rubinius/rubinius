require File.dirname(__FILE__) + '/../../spec_helper'

context "Kernel.String()" do
  specify "should call to_s to convert any arbitrary object to an String" do
    class KernelSpecString
      def to_s; "bar"; end
    end

    String(3).should == '3'
    String("foo").should == 'foo'
    String(nil).should == ''
    String(KernelSpecString.new).should == 'bar'
  end
end

context "Kernel.at_exit()" do
  specify "should fire after all other code" do
    result = `#{RUBY_NAME} -e "at_exit {print 5}; print 6"`
    result.should == "65"
  end

  specify "should fire in reverse order of registration" do
    result = `#{RUBY_NAME} -e "at_exit {print 4};at_exit {print 5}; print 6; at_exit {print 7}"`
    result.should == '6754'
  end
end
