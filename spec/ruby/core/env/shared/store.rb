describe :env_store, :shared => true do
  after(:each) do
    ENV.delete("foo")
  end

  it "sets the environment variable to the given value" do
    ENV.send(@method, "foo", "bar")
    ENV["foo"].should == "bar"
  end

  it "returns the value" do
    value = "bar"
    ENV.send(@method, "foo", value).should equal(value)
  end

  it "deletes the environment variable when the value is nil" do
    ENV["foo"] = "bar"
    ENV.send(@method, "foo", nil)
    ENV.key?("foo").should be_false
  end

  it "coerces the key argument with #to_str" do
    k = mock("key")
    k.should_receive(:to_str).and_return("foo")
    ENV.send(@method, k, "bar")
    ENV["foo"].should == "bar"
  end

  it "coerces the value argument with #to_str" do
    v = mock("value")
    v.should_receive(:to_str).and_return("bar")
    ENV.send(@method, "foo", v)
    ENV["foo"].should == "bar"
  end

  it "raises TypeError when the key is not coercible to String" do
    lambda { ENV.send(@method, Object.new, "bar") }.should raise_error(TypeError)
  end

  it "raises TypeError when the value is not coercible to String" do
    lambda { ENV.send(@method, "foo", Object.new) }.should raise_error(TypeError)
  end

  # The OpenBSD version of setenv allows a symbol to contain the '=' character, or to be the zero-length string.
  platform_is_not :openbsd do
    ruby_version_is ""..."1.9" do
      it "fails silently when the key contains the '=' character" do
	ENV.send(@method, "foo=", "bar")
	ENV.key?("foo=").should be_false
      end

      it "fails silently when the key is an empty string" do
	ENV.send(@method, "", "bar")
	ENV.key?("").should be_false
      end
    end

    ruby_version_is "1.9" do
      it "raises Errno::EINVAL when the key contains the '=' character" do
	lambda { ENV.send(@method, "foo=", "bar") }.should raise_error(Errno::EINVAL)
      end

      it "raises Errno::EINVAL when the key is an empty string" do
	lambda { ENV.send(@method, "", "bar") }.should raise_error(Errno::EINVAL)
      end
    end
  end
  platform_is :openbsd do
    it "correctly stores a value when the key contains the '=' character" do
      ENV.send(@method, "foo=", "bar")
      ENV["foo="].should == "bar"
    end

    it "correctly stores a value when the key is the zero-length string" do
      ENV.send(@method, "", "bar")
      ENV[""].should == "bar"
    end
  end

  it "does nothing when the key is not a valid environment variable key and the value is nil" do
    ENV.send(@method, "foo=", nil)
    ENV.key?("foo=").should be_false
  end
end
