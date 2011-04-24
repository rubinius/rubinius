describe :env_store, :shared => true do
  it "sets the environment variable to the given value" do
    ENV.send(@method, "foo", "bar")
    env.key?("foo").should be_true
    env.value?("bar").should be_true
    ENV.delete "foo"
    ENV["foo"].should be_nil
    ENV.store "foo", "bar"
    env.key?("foo").should be_true
    env.value?("bar").should be_true
    ENV.delete "foo"
  end

  it 'deletes key if value is nil' do
    ENV.send(@method, 'foo', 'bar')
    ENV.has_key?('foo').should be_true
    ENV.send(@method, 'foo', nil)
    ENV.has_key?('foo').should be_false
  end

end
