require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#taint" do
  it "returns self" do
    o = Object.new
    o.taint.should equal(o)
  end

  it "sets the tainted bit" do
    o = Object.new
    o.taint
    o.tainted?.should == true
  end

  ruby_version_is ""..."1.9" do
    it "raises TypeError on an untainted, frozen object" do
      o = Object.new.freeze
      lambda { o.taint }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "raises RuntimeError on an untainted, frozen object" do
      o = Object.new.freeze
      lambda { o.taint }.should raise_error(RuntimeError)
    end
  end

  it "does not raise an error on a tainted, frozen object" do
    o = Object.new.taint.freeze
    o.taint.should equal(o)
  end

  it "has no effect on immediate values" do
    [nil, true, false].each do |v|
      v.taint
      v.tainted?.should == false
    end
  end

  describe "on a Fixnum" do
    before(:each) do
      @fixnum = 1
    end

    ruby_version_is "" ... "2.0" do
      it "has no effect" do
        @fixnum.taint
        @fixnum.tainted?.should be_false
      end
    end

    ruby_version_is "2.0" do
      it "raises a RuntimeError" do
        lambda { @fixnum.taint }.should raise_exception(RuntimeError)
      end
    end
  end

  describe "on a Bignum" do
    before(:each) do
      @bignum = bignum_value
    end

    after(:each) do
      @bignum.untaint if @bignum.tainted?
    end

    ruby_version_is "" ... "2.0" do
      it "taints the bignum" do
        @bignum.taint
        @bignum.tainted?.should be_true
      end
    end

    ruby_version_is "2.0" do
      it "raises a RuntimeError" do
        lambda { @bignum.taint }.should raise_exception(RuntimeError)
      end
    end
  end

  describe "on a Float" do
    before(:each) do
      @float = 0.1
    end

    after(:each) do
      @float.untaint if @float.tainted?
    end

    ruby_version_is "" ... "2.0" do
      it "taints the float" do
        @float.taint
        @float.tainted?.should be_true
      end
    end

    ruby_version_is "2.0" do
      it "raises a RuntimeError" do
        lambda { @float.taint }.should raise_exception(RuntimeError)
      end
    end
  end

  describe "on a Symbol" do
    before(:each) do
      @symbol = :symbol
    end

    ruby_version_is "" ... "2.1" do
      it "has no effect" do
        @symbol.taint
        @symbol.tainted?.should be_false
      end
    end

    ruby_version_is "2.1" do
      it "raises a RuntimeError" do
        lambda { @symbol.taint }.should raise_exception(RuntimeError)
      end
    end
  end
end
