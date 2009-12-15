require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#private" do
  it "makes the target method uncallable from other types" do
    obj = Object.new
    class << obj
      def foo; true; end
    end

    obj.foo.should == true

    class << obj
      private :foo
    end

    lambda { obj.foo }.should raise_error(NoMethodError)
  end
end
