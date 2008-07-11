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
      name = @method.send(cmd).sub(/0x\w+/, '0xXXXXXX')

      deviates_on(:rubinius) do
        name.should == "#<Method: MethodSpecs::MySub#bar (defined in MethodSpecs::MyMod)>"
      end
      compliant_on(:ruby, :jruby) do
        name.should == "#<Method: MethodSpecs::MySub(MethodSpecs::MyMod)#bar>"
      end
    end
  end
end
