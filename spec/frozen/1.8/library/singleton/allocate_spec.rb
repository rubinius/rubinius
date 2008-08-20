require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton.allocate" do
  it "is a private method" do
    lambda { SingletonSpecs::MyClass.allocate }.should raise_error(NoMethodError)
  end
end
