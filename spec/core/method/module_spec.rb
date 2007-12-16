require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

extension :rubinius do
  describe "Method#module" do
    it "returns the module with which the method is instantiated" do
      MethodSpecs::D.new.method(:bar).module.should == MethodSpecs::D
    end
  end
end
