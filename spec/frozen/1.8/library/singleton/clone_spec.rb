require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton#clone" do
  it "is prevented" do
    lambda { SingletonSpecs::MyClass.instance.clone }.should raise_error(TypeError)
  end
end
