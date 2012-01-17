describe :string_replace, :shared => true do
  it "returns self" do
    a = "a"
    a.send(@method, "b").should equal(a)
  end

  it "replaces the content of self with other" do
    a = "some string"
    a.send(@method, "another string")
    a.should == "another string"
  end

  it "replaces the taint status of self with that of other" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.send(@method, b)
    a.tainted?.should == true
  end

  it "tries to convert other to string using to_str" do
    other = mock('x')
    other.should_receive(:to_str).and_return("converted to a string")
    "hello".send(@method, other).should == "converted to a string"
  end

  it "raises a TypeError if other can't be converted to string" do
    lambda { "hello".send(@method, 123)       }.should raise_error(TypeError)
    lambda { "hello".send(@method, [])        }.should raise_error(TypeError)
    lambda { "hello".send(@method, mock('x')) }.should raise_error(TypeError)
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError on a frozen instance that is modified" do
      a = "hello".freeze
      lambda { a.send(@method, "world") }.should raise_error(TypeError)
    end

    it "does not raise an exception on a frozen instance when self-replacing" do
      a = "hello".freeze
      a.send(@method, a).should equal(a)
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError on a frozen instance that is modified" do
      a = "hello".freeze
      lambda { a.send(@method, "world") }.should raise_error(RuntimeError)
    end

    # see [ruby-core:23666]
    it "raises a RuntimeError on a frozen instance when self-replacing" do
      a = "hello".freeze
      lambda { a.send(@method, a) }.should raise_error(RuntimeError)
    end
  end
end
