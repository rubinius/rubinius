require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#frozen?" do
  it "returns true if self is frozen" do
    o = mock('o')
    p = mock('p')
    p.freeze
    o.frozen?.should == false
    p.frozen?.should == true
  end

  describe "on integers" do
    before(:each) do
      @fixnum = 1
      @bignum = bignum_value
    end

    ruby_version_is "" ... "2.0" do
      it "returns false" do
        @fixnum.frozen?.should be_false
        @bignum.frozen?.should be_false
      end
    end

    ruby_version_is "2.0" do
      it "returns true" do
        @fixnum.frozen?.should be_true
        @bignum.frozen?.should be_true
      end
    end
  end

  describe "on a Float" do
    before(:each) do
      @float = 0.1
    end

    ruby_version_is "" ... "2.0" do
      it "returns false" do
        @float.frozen?.should be_false
      end
    end

    ruby_version_is "2.0" do
      it "returns true" do
        @float.frozen?.should be_true
      end
    end
  end

  describe "on a Symbol" do
    before(:each) do
      @symbol = :symbol
    end

    ruby_version_is "" ... "2.1" do
      it "returns false" do
        @symbol.frozen?.should be_false
      end
    end

    ruby_version_is "2.1" do
      it "returns true" do
        @symbol.frozen?.should be_true
      end
    end
  end
end
