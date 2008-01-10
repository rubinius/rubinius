require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton.instance" do
  it "returns an instance of the singleton class" do
    SingletonSpecs::MyClass.instance.should be_kind_of(SingletonSpecs::MyClass)
  end
end
