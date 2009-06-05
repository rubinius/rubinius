describe :string_concat, :shared => true do
  it "concatenates the given argument to self and returns self" do
    str = 'hello '
    str.send(@method, 'world').should equal(str)
    str.should == "hello world"
  end

  it "converts the given argument to a String using to_str" do
    obj = mock('world!')
    obj.should_receive(:to_str).and_return("world!")
    a = 'hello '.send(@method, obj)
    a.should == 'hello world!'
  end

  it "raises a TypeError if the given argument can't be converted to a String" do
    lambda { a = 'hello '.send(@method, [])        }.should raise_error(TypeError)
    lambda { a = 'hello '.send(@method, mock('x')) }.should raise_error(TypeError)
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError when self is frozen" do
      a = "hello"
      a.freeze

      lambda { a.send(@method, "")     }.should raise_error(TypeError)
      lambda { a.send(@method, "test") }.should raise_error(TypeError)
    end
  end
  ruby_version_is "1.9" do
    it "raises a RuntimeError when self is frozen" do
      a = "hello"
      a.freeze

      lambda { a.send(@method, "")     }.should raise_error(RuntimeError)
      lambda { a.send(@method, "test") }.should raise_error(RuntimeError)
    end
  end

  ruby_version_is "1.9" do
    it "raises ArgumentError for negative length argument" do
      lambda do
        'glark'.force_encoding('UTF-8').send(@method, -200) 
      end.should raise_error(ArgumentError)
    end
  end

  it "works when given a subclass instance" do
    a = "hello"
    a << StringSpecs::MyString.new(" world")
    a.should == "hello world"
  end

  it "taints self if other is tainted" do
    x = "x"
    x.send(@method, "".taint).tainted?.should == true

    x = "x"
    x.send(@method, "y".taint).tainted?.should == true
  end
end

describe :string_concat_fixnum, :shared => true do
  it "converts the given Fixnum to a char before concatenating" do
    b = 'hello '.send(@method, 'world').send(@method, 33)
    b.should == "hello world!"
    b.send(@method, 0)
    b.should == "hello world!\x00"
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError when the given Fixnum is not between 0 and 255" do
      lambda { "hello world" << 333 }.should raise_error(TypeError)
      lambda { "".send(@method, (256 * 3 + 64)) }.should raise_error(TypeError)
      lambda { "".send(@method, -200)           }.should raise_error(TypeError)
    end
  end

  it "doesn't call to_int on its argument" do
    x = mock('x')
    x.should_not_receive(:to_int)

    lambda { "".send(@method, x) }.should raise_error(TypeError)
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError when self is frozen" do
      a = "hello"
      a.freeze

      lambda { a.send(@method, 0)  }.should raise_error(TypeError)
      lambda { a.send(@method, 33) }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError when self is frozen" do
      a = "hello"
      a.freeze

      lambda { a.send(@method, 0)  }.should raise_error(RuntimeError)
      lambda { a.send(@method, 33) }.should raise_error(RuntimeError)
    end
  end    
end
