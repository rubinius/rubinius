require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "String#strip" do
  it "returns a new string with leading and trailing whitespace removed" do
    "   hello   ".strip.should == "hello"
    "   hello world   ".strip.should == "hello world"
    "\tgoodbye\r\v\n".strip.should == "goodbye"
    "  goodbye \000".strip.should == "goodbye"
  end

  it "taints the result when self is tainted" do
    "".taint.strip.tainted?.should == true
    "ok".taint.strip.tainted?.should == true
    "  ok  ".taint.strip.tainted?.should == true
  end
end

describe "String#strip!" do
  it "modifies self in place and returns self" do
    a = "   hello   "
    a.strip!.should equal(a)
    a.should == "hello"
  end

  it "returns nil if no modifications where made" do
    a = "hello"
    a.strip!.should == nil
    a.should == "hello"
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError on a frozen instance that is modified" do
      lambda { "  hello  ".freeze.strip! }.should raise_error(TypeError)
    end

    it "does not raise an exception on a frozen instance that would not be modified" do
      "hello".freeze.strip!.should be_nil
      "".freeze.strip!.should be_nil
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError on a frozen instance that is modified" do
      lambda { "  hello  ".freeze.strip! }.should raise_error(RuntimeError)
    end

    # see #1552
    it "raises a RuntimeError on a frozen instance that would not be modified" do
      lambda {"hello".freeze.strip! }.should raise_error(RuntimeError)
      lambda {"".freeze.strip!      }.should raise_error(RuntimeError)
    end
  end
end
