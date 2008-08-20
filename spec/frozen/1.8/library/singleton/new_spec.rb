require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton.new" do
  it "is a private method" do
    lambda { SingletonSpecs::NewSpec.new }.should raise_error(NoMethodError)
  end
end
