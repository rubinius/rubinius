require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton._load" do

  it "is a private method" do
    lambda { SingletonSpecs::MyClass.instance._load("") }.should raise_error(NoMethodError)
  end
  
  it "returns the singleton instance for anything passed in" do
    SingletonSpecs::MyClass.send(:_load, "").equal?(SingletonSpecs::MyClass.instance).should == true
    SingletonSpecs::MyClass.send(:_load, "42").equal?(SingletonSpecs::MyClass.instance).should == true
    SingletonSpecs::MyClass.send(:_load, 42).equal?(SingletonSpecs::MyClass.instance).should == true
  end

  it "returns the singleton instance for anything passed in for a singleton subclass" do
    SingletonSpecs::MyClassChild.send(:_load, "").equal?(SingletonSpecs::MyClassChild.instance).should == true
    SingletonSpecs::MyClassChild.send(:_load, "42").equal?(SingletonSpecs::MyClassChild.instance).should == true
    SingletonSpecs::MyClassChild.send(:_load, 42).equal?(SingletonSpecs::MyClassChild.instance).should == true
  end

  it "returns the singleton instance for anything passed in for a singleton clone" do
    klone = SingletonSpecs::MyClass.clone
    klone.send(:_load, "").equal?(klone.instance).should == true
    klone.send(:_load, "42").equal?(klone.instance).should == true
    klone.send(:_load, 42).equal?(klone.instance).should == true
  end

end