require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "UnboundMethod#name" do
  ruby_version_is '1.8.7' do
    it "returns the name of the method" do
      String.instance_method(:upcase).name.should == :upcase
    end
    
    it "returns the name even when aliased" do
      obj = UnboundMethodSpecs::Methods.new
      obj.method(:foo).unbind.name.should == :foo
      obj.method(:bar).unbind.name.should == :bar
      UnboundMethodSpecs::Methods.instance_method(:bar).name.should == :bar
    end
  end
end