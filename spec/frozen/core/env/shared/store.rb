describe :env_store, :shared => true do
  it "sets the environment variable to the given value" do
    ENV.send(@method, "foo", "bar")
    env.key?("foo").should == true
    env.value?("bar").should == true
    ENV.delete "foo"
    ENV["foo"].should == nil
    ENV.store "foo", "bar"
    env.key?("foo").should == true
    env.value?("bar").should == true
    ENV.delete "foo"
  end
end
