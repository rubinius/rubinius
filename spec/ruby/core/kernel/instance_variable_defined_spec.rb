require File.expand_path('../../../spec_helper', __FILE__)

module KernelSpec
  class Foo
    def initialize
      @bar = 'this is an instance variable'
    end
  end
end

describe "Kernel#instance_variable_defined?" do
  it 'returns false if the instance variable is not defined' do
    foo = KernelSpec::Foo.new
    foo.instance_variable_defined?("@foo").should == false
    foo.instance_variable_defined?(:@foo).should == false
  end

  it 'returns true if the instance variable is defined' do
    foo = KernelSpec::Foo.new
    foo.instance_variable_defined?("@bar").should == true
    foo.instance_variable_defined?(:@bar).should == true
  end

end
