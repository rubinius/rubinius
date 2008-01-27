require "#{File.dirname __FILE__}/../../../spec_helper"
require "#{File.dirname __FILE__}/../fixtures/classes"


shared :unboundmethod_to_s do |cmd|
  describe "UnboundMethod##{cmd}" do
    before :each do
      @from_module = UnboundMethodSpecs::Methods.instance_method(:from_mod)
      @from_method = UnboundMethodSpecs::Methods.new.method(:from_mod).unbind
    end

    it "returns a String" do
      @from_module.send(cmd).class.should == String
      @from_method.send(cmd).class.should == String
    end

    it "the String reflects that this is an UnboundMethod object" do
      @from_module.send(cmd).should =~ /\bUnboundMethod\b/
      @from_method.send(cmd).should =~ /\bUnboundMethod\b/
    end

    it "the String shows the method name, Module defined in and Module extracted from" do
      @from_module.send(cmd).should =~ /\bfrom_mod\b/
      @from_module.send(cmd).should =~ /\bUnboundMethodSpecs::Mod\b/
      @from_method.send(cmd).should =~ /\bUnboundMethodSpecs::Methods\b/
    end
  end
end
