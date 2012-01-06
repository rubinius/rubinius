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
    lambda { 'hello '.send(@method, [])        }.should raise_error(TypeError)
    lambda { 'hello '.send(@method, mock('x')) }.should raise_error(TypeError)
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

  it "works when given a subclass instance" do
    a = "hello"
    a << StringSpecs::MyString.new(" world")
    a.should == "hello world"
  end

  it "taints self if other is tainted" do
    "x".send(@method, "".taint).tainted?.should == true
    "x".send(@method, "y".taint).tainted?.should == true
  end

  ruby_version_is "1.9" do
    it "untrusts self if other is untrusted" do
      "x".send(@method, "".untrust).untrusted?.should == true
      "x".send(@method, "y".untrust).untrusted?.should == true
    end
  end

  describe "with Integer" do
    ruby_version_is ""..."1.9" do
      it "concatencates the argument interpreted as an ASCII character" do
        b = 'hello '.send(@method, 'world').send(@method, 33)
        b.should == "hello world!"
        b.send(@method, 0)
        b.should == "hello world!\x00"
      end

      it "raises a TypeError when the argument is not between 0 and 255" do
        lambda { "".send(@method, -200)         }.should raise_error(TypeError)
        lambda { "".send(@method, 256)          }.should raise_error(TypeError)
        lambda { "".send(@method, bignum_value) }.should raise_error(TypeError)
      end
    end

    ruby_version_is "1.9" do
      it "concatencates the argument interpreted as a codepoint" do
        b = "".send(@method, 33)
        b.should == "!"

        b.encode!(Encoding::UTF_8)
        b.send(@method, 0x203D)
        b.should == "!\u203D"
      end

      it "raises RangeError if the argument is an invalid codepoint for self's encoding" do
        lambda { "".encode(Encoding::US_ASCII) << 256 }.should raise_error(RangeError)
        lambda { "".encode(Encoding::EUC_JP) << 0x81  }.should raise_error(RangeError)
      end

      it "raises RangeError if the argument is negative" do
        lambda { "".send(@method, -200)          }.should raise_error(RangeError)
        lambda { "".send(@method, -bignum_value) }.should raise_error(RangeError)
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
end
