require "#{File.dirname __FILE__}/../../../spec_helper"
require "#{File.dirname __FILE__}/../fixtures/classes"

describe :method_to_s, :shared => true do
  before :each do
    @m = MethodSpecs::MySub.new.method :bar
  end

  it "returns a String" do
    @m.send(@method).class.should == String
  end

  it "the String reflects that this is a Method object" do
    @m.send(@method).should =~ /\bMethod\b/
  end

  it "the String shows the method name, Module defined in and Module extracted from" do
    name = @m.send(@method).sub(/0x\w+/, '0xXXXXXX')

    deviates_on(:rubinius) do
      name.should == "#<Method: MethodSpecs::MySub#bar (defined in MethodSpecs::MyMod)>"
    end

    compliant_on(:ruby, :jruby) do
      name.should == "#<Method: MethodSpecs::MySub(MethodSpecs::MyMod)#bar>"
    end
  end
end
