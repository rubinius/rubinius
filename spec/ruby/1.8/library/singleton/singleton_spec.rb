require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton" do
  it "returns the same object for any instance of the class" do
    a = SingletonSpecs::MyClass.instance
    b = SingletonSpecs::MyClass.instance
    a.data = 123

    a.should == b
    b.data.should == 123
  end
end
