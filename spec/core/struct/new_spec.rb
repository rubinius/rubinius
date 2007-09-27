require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct.new" do
  failure :rubinius do
    it "creates a constant in Struct namespace with string as first argument" do
      # TODO: should pass, rbx bug
      Struct.new('Animal', :name, :legs, :eyeballs).should == Struct::Animal

      struct = Struct.new('Animal', :name, :legs, :eyeballs)
      struct.should == Struct::Animal
    end
  end

  it "overwrites previously defined constants with string as first argument" do
    first = Struct.new('Person', :height, :weight)
    first.should == Struct::Person

    second = Struct.new('Person', :hair, :sex)
    second.should == Struct::Person

    first.members.should_not == second.members
  end

  it "calls to_str on its first argument (constant name)" do
    obj = Object.new
    def obj.to_str() "Foo" end
    Struct.new(obj).should == Struct::Foo
  end

  it "accepts nil first argument for not creating constants" do
    Struct.new(nil, :foo).new("foo").foo.should == "foo"
  end

  it "does not create a constant with symbol as first argument" do
    Struct.new(:Animal, :name, :legs, :eyeballs).should_not == Struct::Animal
  end

  it "creates a new anonymous class with symbol arguments" do
    Struct.new(:make, :model).class.should == Class
  end

  it "fails with invalid constant name as first argument" do
    should_raise(NameError) { Struct.new('animal', :name, :legs, :eyeballs) }
  end

  it "raises ArgumentError if object#to_sym is nil" do
    should_raise(ArgumentError) { Struct.new(:animal, 10000) }
  end
  
  it "raises TypeError if object doesn't respond to to_sym" do
    should_raise(TypeError) { Struct.new(:animal, 1.0) }
    should_raise(TypeError) { Struct.new(:animal, Time.now) }
    should_raise(TypeError) { Struct.new(:animal, Class) }
    should_raise(TypeError) { Struct.new(:animal, nil) }
    should_raise(TypeError) { Struct.new(:animal, true) }
    should_raise(TypeError) { Struct.new(:animal, ['chris', 'evan']) }
    should_raise(TypeError) { Struct.new(:animal, { :name => 'chris' }) }
  end

  it "instance_eval's a passed block" do
    klass = Struct.new(:something) { @something_else = 'something else entirely!' }
    klass.instance_variables.should_include '@something_else'
  end

  it "creates a constant in subclass' namespace" do
    Apple.new('Computer', :size).should == Apple::Computer
  end

  it "creates an instance" do
    Struct::Ruby.new.kind_of?(Struct::Ruby).should == true
  end

  it "creates reader methods" do
    Struct::Ruby.new.methods.should_include 'version'
    Struct::Ruby.new.methods.should_include 'platform'
  end

  it "creates writer methods" do
    Struct::Ruby.new.methods.should_include 'version='
    Struct::Ruby.new.methods.should_include 'platform='
  end

  it "fails with too many arguments" do
    should_raise(ArgumentError) { Struct::Ruby.new('2.0', 'i686', true) }
  end
end
