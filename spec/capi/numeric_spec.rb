require File.dirname(__FILE__) + '/spec_helper'

load_extension("numeric")

describe "CApiNumericSpecs" do
  before :each do
    @s = CApiNumericSpecs.new
  end

  describe "rb_num2long" do
    it "raises an TypeError if passed nil" do
      lambda { @s.rb_num2long(nil) }.should raise_error(TypeError)
    end

    it "converts a Float" do
      @s.rb_num2long(4.2).should == 4
    end

    it "converts a Bignum" do
      @s.rb_num2long(0x7fff_ffff).should == 0x7fff_ffff
    end

    it "converts a Fixnum" do
      @s.rb_num2long(5).should == 5
    end

    it "calls #to_int to coerce the value" do
      obj = mock("number")
      obj.should_receive(:to_int).and_return(2)
      @s.rb_num2long(obj).should == 2
    end
  end

  describe "rb_num2ulong" do
    it "raises an TypeError if passed nil" do
      lambda { @s.rb_num2ulong(nil) }.should raise_error(TypeError)
    end

    it "converts a Float" do
      @s.rb_num2ulong(4.2).should == 4
    end

    it "converts a Bignum" do
      @s.rb_num2ulong(0xffff_ffff).should == 0xffff_ffff
    end

    it "converts a Fixnum" do
      @s.rb_num2ulong(5).should == 5
    end

    it "calls #to_int to coerce the value" do
      obj = mock("number")
      obj.should_receive(:to_int).and_return(2)
      @s.rb_num2ulong(obj).should == 2
    end
  end

  describe "rb_Integer" do
    it "should create a new Integer from a String" do
      i = @s.rb_Integer("8675309")
      i.should be_kind_of(Integer)
      i.should eql(8675309)
    end
  end

  describe "rb_ll2inum" do
    it "should create a new Fixnum from a small signed long long" do
      i = @s.rb_ll2inum_14()
      i.should be_kind_of(Fixnum)
      i.should eql(14)
    end
  end

  describe "rb_int2inum" do
    it "should create a new Fixnum from a long" do
      i = @s.rb_int2inum_14()
      i.should be_kind_of(Fixnum)
      i.should eql(14)
    end
  end

  describe "rb_num2dbl" do
    it "raises an TypeError if passed nil" do
      lambda { @s.rb_num2dbl(nil) }.should raise_error(TypeError)
    end

    it "raises an TypeError if passed a String" do
      lambda { @s.rb_num2dbl("1.2") }.should raise_error(TypeError)
    end

    it "converts a Float" do
      @s.rb_num2dbl(4.2).should == 4.2
    end

    it "converts a Bignum" do
      @s.rb_num2dbl(2**70).should == (2**70).to_f
    end

    it "converts a Fixnum" do
      @s.rb_num2dbl(5).should == 5.0
    end

    it "calls #to_f to coerce the value" do
      obj = mock("number")
      obj.should_receive(:to_f).and_return(2.0)
      @s.rb_num2dbl(obj).should == 2.0
    end
  end
end
