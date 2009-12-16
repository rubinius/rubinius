require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "UnboundMethod#owner" do
  ruby_version_is '1.8.7' do
    it "returns the owner of the method" do
      "abc".method(:upcase).owner.should == String
    end
    
    it "returns the name even when aliased" do
      obj = UnboundMethodSpecs::Methods.new
      obj.method(:foo).owner.should == UnboundMethodSpecs::Methods
      obj.method(:bar).owner.should == UnboundMethodSpecs::Methods
    end

    it "returns the class/module it was defined in" do
      UnboundMethodSpecs::C.new.method(:baz).owner.should == UnboundMethodSpecs::A
      UnboundMethodSpecs::Methods.new.method(:from_mod).owner.should == UnboundMethodSpecs::Mod
    end

  end
end