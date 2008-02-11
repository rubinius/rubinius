require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton.instance" do
  it "returns an instance of the singleton class" do
    SingletonSpecs::MyClass.instance.should be_kind_of(SingletonSpecs::MyClass)
  end
  
  it "returns the same instance for multiple calls to instance" do
    SingletonSpecs::MyClass.instance.equal?(SingletonSpecs::MyClass.instance).should == true
  end
  
  it "returns an instance of the singleton's subclasses" do
    SingletonSpecs::MyClassChild.instance.should be_kind_of(SingletonSpecs::MyClassChild)
  end
  
  it "returns the same instance for multiple class to instance on subclasses" do
    SingletonSpecs::MyClassChild.instance.equal?(SingletonSpecs::MyClassChild.instance).should == true
  end
  
  it "returns an instance of the singleton's clone" do
    klone = SingletonSpecs::MyClassChild.clone
    klone.instance.should be_kind_of(klone)
  end
  
  it "returns the same instance for multiple class to instance on clones" do
    klone = SingletonSpecs::MyClassChild.clone
    klone.instance.equal?(klone.instance).should == true
  end  
end
