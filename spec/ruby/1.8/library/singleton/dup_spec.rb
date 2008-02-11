require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton#dup" do
  it "is prevented" do
    lambda { SingletonSpecs::MyClass.instance.dup }.should raise_error(TypeError)
  end
end
