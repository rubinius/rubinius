require File.expand_path('../../../spec_helper', __FILE__)

describe "Numeric#-@" do
  it " should return the same value with opposite sign (integers)" do
    0.send(:-@).should == 0
    100.send(:-@).should == -100
    -100.send(:-@).should == 100
  end

  it " should return the same value with opposite sign (floats)" do
    34.56.send(:-@).should == -34.56
    -34.56.send(:-@).should == 34.56
  end

  it " should return the same value with opposite sign (two complement)" do
    2147483648.send(:-@).should == -2147483648
    -2147483648.send(:-@).should == 2147483648
    9223372036854775808.send(:-@).should == -9223372036854775808
    -9223372036854775808.send(:-@).should == 9223372036854775808
  end
end
