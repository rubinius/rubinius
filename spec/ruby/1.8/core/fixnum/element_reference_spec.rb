require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#[]" do
  it "returns the nth bit in the binary representation of self" do
    2[3].should == 0
    15[1].should == 1

    2[3].should == 0
    3[0xffffffff].should == 0
    3[-0xffffffff].should == 0
  end
  
  it "tries to convert the given argument to an Integer using #to_int" do
    15[1.3].should == 15[1]
    
    (obj = mock('1')).should_receive(:to_int).and_return(1)
    2[obj].should == 1
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { 3[obj] }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { 3[obj] }.should raise_error(TypeError)
  end

  # On Rubinius this works ok. This failure is actually a bug in MRI, because there is
  # no reason it should fail, especially because it can do & on a fixnum and bignum.
  # See http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-core/15838
  not_compliant_on :rubinius do
    it "raises a RangeError when the given argument is out of range of Integer" do
      (obj = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
      lambda { 3[obj] }.should raise_error(RangeError)

      obj = 8e19
      lambda { 3[obj] }.should raise_error(RangeError)
    end
  end
  
  deviates_on :rubinius do
    it "coerces arguments correctly even if it is a Bignum" do
      (obj = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
      3[obj].should == 0
      obj = 8e19
      
      3[obj].should == 0
    end
  end

end
