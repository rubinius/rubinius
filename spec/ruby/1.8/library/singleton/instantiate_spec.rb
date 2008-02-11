require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton._instantiate?" do

  it "is private" do
    lambda { SingletonSpecs::MyClass._instantiate? }.should raise_error(NoMethodError)
  end

  it "returns nil until it is instantiated" do
    SingletonSpecs::NotInstantiated.send(:_instantiate?).should == nil
    SingletonSpecs::NotInstantiated.instance
    SingletonSpecs::NotInstantiated.send(:_instantiate?).eql?(SingletonSpecs::NotInstantiated.instance).should == true
  end

end