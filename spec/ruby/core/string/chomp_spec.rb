require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "String#chomp" do
  it "returns a new string with the given record separator removed" do
    "hello".chomp("llo").should == "he"
    "hellollo".chomp("llo").should == "hello"
  end

  it "removes carriage return (except \\r) chars multiple times when separator is an empty string" do
    "".chomp("").should == ""
    "hello".chomp("").should == "hello"
    "hello\n".chomp("").should == "hello"
    "hello\nx".chomp("").should == "hello\nx"
    "hello\r\n".chomp("").should == "hello"
    "hello\r\n\r\n\n\n\r\n".chomp("").should == "hello"

    "hello\r".chomp("").should == "hello\r"
    "hello\n\r".chomp("").should == "hello\n\r"
    "hello\r\r\r\n".chomp("").should == "hello\r\r"
  end

  it "removes carriage return chars (\\n, \\r, \\r\\n) when separator is \\n" do
    "hello".chomp("\n").should == "hello"
    "hello\n".chomp("\n").should == "hello"
    "hello\r\n".chomp("\n").should == "hello"
    "hello\n\r".chomp("\n").should == "hello\n"
    "hello\r".chomp("\n").should == "hello"
    "hello \n there".chomp("\n").should == "hello \n there"
    "hello\r\n\r\n\n\n\r\n".chomp("\n").should == "hello\r\n\r\n\n\n"

    "hello\n\r".chomp("\r").should == "hello\n"
    "hello\n\r\n".chomp("\r\n").should == "hello\n"
  end

  it "returns self if the separator is nil" do
    "hello\n\n".chomp(nil).should == "hello\n\n"
  end

  it "returns an empty string when called on an empty string" do
    "".chomp("\n").should == ""
    "".chomp("\r").should == ""
    "".chomp("").should == ""
    "".chomp(nil).should == ""
  end

  describe "with a different $/ separator" do
    before :each do
      @old_rec_sep = $/
      @expectation = lambda do |sep|
        $/ = sep
        [ ["", "".chomp(sep)],
          ["x", "x".chomp(sep)],
          ["x\n", "x\n".chomp(sep)],
          ["x\r", "x\r".chomp(sep)],
          ["x\r\n", "x\r\n".chomp(sep)],
          ["x\n\r\r\n", "x\n\r\r\n".chomp(sep)],
          ["hello", "hello".chomp(sep)]
        ].should be_computed_by(:'chomp')
      end
    end

    after :each do
      $/ = @old_rec_sep
    end

    it "uses separator #{"".inspect} for $/ as the separator when none is given" do
      @expectation.call("")
    end

    it "uses separator #{"llo".inspect} for $/ as the separator when none is given" do
      @expectation.call("llo")
    end

    it "uses separator #{"\n".inspect} for $/ as the separator when none is given" do
      @expectation.call("\n")
    end

    it "uses separator #{"\r".inspect} for $/ as the separator when none is given" do
      @expectation.call("\r")
    end

    it "uses separator #{nil.inspect} for $/ as the separator when none is given" do
      @expectation.call(nil)
    end
  end

  it "taints result when self is tainted" do
    "hello".taint.chomp("llo").tainted?.should == true
    "hello".taint.chomp("").tainted?.should == true
    "hello".taint.chomp(nil).tainted?.should == true
    "hello".taint.chomp.tainted?.should == true
    "hello\n".taint.chomp.tainted?.should == true

    "hello".chomp("llo".taint).tainted?.should == false
  end

  it "calls #to_str to convert separator to a String" do
    separator = mock('llo')
    separator.should_receive(:to_str).and_return("llo")

    "hello".chomp(separator).should == "he"
  end

  it "raises a TypeError if separator can't be converted to a string" do
    lambda { "hello".chomp(30.3)      }.should raise_error(TypeError)
    lambda { "hello".chomp([])        }.should raise_error(TypeError)
    lambda { "hello".chomp(mock('x')) }.should raise_error(TypeError)
  end

  it "returns subclass instances when called on a subclass" do
    StringSpecs::MyString.new("hello\n").chomp.should be_kind_of(StringSpecs::MyString)
    StringSpecs::MyString.new("hello").chomp.should be_kind_of(StringSpecs::MyString)
    StringSpecs::MyString.new("").chomp.should be_kind_of(StringSpecs::MyString)
  end
end

describe "String#chomp!" do
  it "modifies self in place and returns self" do
    s = "one\n"
    s.chomp!.should equal(s)
    s.should == "one"

    t = "two\r\n"
    t.chomp!.should equal(t)
    t.should == "two"

    u = "three\r"
    u.chomp!
    u.should == "three"

    v = "four\n\r"
    v.chomp!
    v.should == "four\n"

    w = "five\n\n"
    w.chomp!(nil)
    w.should == "five\n\n"

    x = "six"
    x.chomp!("ix")
    x.should == "s"

    y = "seven\n\n\n\n"
    y.chomp!("")
    y.should == "seven"
  end

  it "returns nil if no modifications were made" do
     v = "four"
     v.chomp!.should == nil
     v.should == "four"

    "".chomp!.should == nil
    "line".chomp!.should == nil

    "hello\n".chomp!("x").should == nil
    "hello".chomp!("").should == nil
    "hello".chomp!(nil).should == nil
  end

  describe "with a different $/ separator" do
    before :each do
      @old_rec_sep = $/
      @expectation = lambda do |sep|
        $/ = sep
        [ ["", "".chomp!(sep)],
          ["x", "x".chomp!(sep)],
          ["x\n", "x\n".chomp!(sep)],
          ["x\r", "x\r".chomp!(sep)],
          ["x\r\n", "x\r\n".chomp!(sep)],
          ["x\n\r\r\n", "x\n\r\r\n".chomp!(sep)],
          ["hello", "hello".chomp!(sep)]
        ].should be_computed_by(:'chomp!')
      end
    end

    after :each do
      $/ = @old_rec_sep
    end

    it "uses separator #{"".inspect} for $/ as the separator when none is given" do
      @expectation.call("")
    end

    it "uses separator #{"llo".inspect} for $/ as the separator when none is given" do
      @expectation.call("llo")
    end

    it "uses separator #{"\n".inspect} for $/ as the separator when none is given" do
      @expectation.call("\n")
    end

    it "uses separator #{"\r".inspect} for $/ as the separator when none is given" do
      @expectation.call("\r")
    end

    it "uses separator #{nil.inspect} for $/ as the separator when none is given" do
      @expectation.call(nil)
    end
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError when self is frozen" do
      a = "string\n"
      a.freeze

      lambda { a.chomp! }.should raise_error(TypeError)
      lambda { a.chomp!("\n") }.should raise_error(TypeError)
      lambda { a.chomp!("") }.should raise_error(TypeError)

      c = "fooa"
      c.freeze
      lambda { c.chomp!("a") }.should raise_error(TypeError)
    end

    it "does raise an exception when no change would be done and no argument is passed in" do
      b = "string"
      b.freeze

      lambda { b.chomp! }.should raise_error(TypeError)
    end

    it "does not raise an exception when no change would be done and no argument is passed in on an empty string" do
      b = ""
      b.freeze

       b.chomp!.should be_nil
    end

    it "does not raise an exception when the string would not be modified" do
      a = "string\n\r"
      a.freeze

      a.chomp!(nil).should be_nil
      a.chomp!("x").should be_nil
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError on a frozen instance when it is modified" do
      a = "string\n\r"
      a.freeze

      lambda { a.chomp! }.should raise_error(RuntimeError)
    end

    # see [ruby-core:23666]
    it "raises a RuntimeError on a frozen instance when it would not be modified" do
      a = "string\n\r"
      a.freeze
      lambda { a.chomp!(nil) }.should raise_error(RuntimeError)
      lambda { a.chomp!("x") }.should raise_error(RuntimeError)
    end
  end
end
