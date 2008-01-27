require "#{File.dirname __FILE__}/../../../spec_helper"
require "#{File.dirname __FILE__}/../fixtures/classes"

shared :method_to_s do |cmd|
  describe "Method##{cmd}" do
    before :each do
      @method = MethodSpecs::MySub.new.method :bar
    end

    it "returns a String" do
      @method.send(cmd).class.should == String
    end

    it "the String reflects that this is a Method object" do
      @method.send(cmd).should =~ /\bMethod\b/
    end

    it "the String shows the method name, Module defined in and Module extracted from" do
      @method.send(cmd).should =~ /\bbar\b/
      @method.send(cmd).should =~ /\bMethodSpecs::MyMod\b/
      @method.send(cmd).should =~ /\bMethodSpecs::MySub\b/
    end
  end
end
