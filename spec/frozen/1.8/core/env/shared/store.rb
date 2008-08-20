require File.dirname(__FILE__) + '/../fixtures/classes'

describe :env_store, :shared => true do
  it "sets the environment variable to the given value" do
    ENV.send(@method, "foo", "bar")
    EnvSpecs.get_env.key?("foo").should == true
    EnvSpecs.get_env.value?("bar").should == true
    ENV.delete "foo"
    ENV["foo"].should == nil
    ENV.store "foo", "bar"
    EnvSpecs.get_env.key?("foo").should == true
    EnvSpecs.get_env.value?("bar").should == true
    ENV.delete "foo"
  end
end
