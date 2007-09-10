require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#**" do
  it "returns self raise to the other power" do
    (2 ** 3).should == 8
  end

  it "coerces fixnum and return self raise to the other power" do
    (5 ** -1).to_f.to_s.should == '0.2'
    (9 ** 0.5).to_s.should == '3.0'
  end

  it "automatically converts to Bignum when necessary" do
    (0..40).map {|x| 2**x}.should == [
      1, 2, 4, 8, 16, 32, 64, 128,
      256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
      65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608,
      16777216, 33554432, 67108864, 134217728, 268435456, 536870912,
      1073741824, 2147483648, 4294967296, 8589934592, 17179869184,
      34359738368, 68719476736, 137438953472, 274877906944, 549755813888,
      1099511627776
    ]
  end
end
