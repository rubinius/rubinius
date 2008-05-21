require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# TODO: change to a.should be_equal(b)
# TODO: write spec for cloning classes and calling private methods
# TODO: write spec for private_methods not showing up via extended
describe "Singleton._load" do

  it "is a private method" do
    # TODO: SingletonSpecs::MyClass.private_methods.sort.should include("_load")
    lambda { # TODO: remove when above works
      SingletonSpecs::MyClass._load("")
    }.should raise_error(NoMethodError)
    SingletonSpecs::MyClass.send(:_load, ""  ).should_not == nil
  end

  it "returns the singleton instance for anything passed in" do
    klass = SingletonSpecs::MyClass
    klass.send(:_load, ""  ).should equal(klass.instance)
    klass.send(:_load, "42").should equal(klass.instance)
    klass.send(:_load, 42  ).should equal(klass.instance)
  end

  it "returns the singleton instance for anything passed in to subclass" do
    subklass = SingletonSpecs::MyClassChild
    subklass.send(:_load, ""  ).should equal(subklass.instance)
    subklass.send(:_load, "42").should equal(subklass.instance)
    subklass.send(:_load, 42  ).should equal(subklass.instance)
  end
end
