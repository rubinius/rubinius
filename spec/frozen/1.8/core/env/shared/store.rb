require File.dirname(__FILE__) + '/../fixtures/classes'

shared :env_store do |cmd|
  describe "ENV.#{cmd}" do
    it "sets the environment variable to the given value" do
      ENV.send(cmd, "foo", "bar")
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
end
