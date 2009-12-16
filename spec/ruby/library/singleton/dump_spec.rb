require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton#_dump" do

  it "returns an empty string" do
    SingletonSpecs::MyClass.instance.send(:_dump).should == ""
  end
  
  it "returns an empty string from a singleton subclass" do
    SingletonSpecs::MyClassChild.instance.send(:_dump).should == ""
  end

end
