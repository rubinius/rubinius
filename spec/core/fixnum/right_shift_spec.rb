require File.expand_path('../../../spec_helper', __FILE__)

describe "Fixnum#>> with n >> m" do
  it "raises a RangeError when m > 0 and the Integer argument is not in Fixnum range" do
    lambda { 3 >> (fixnum_max() + 1) }.should raise_error(RangeError)
  end

  it "raises a RangeError when m > 0 the Float argument is not in Fixnum range" do
    lambda { 3 >> (fixnum_max() + 1).to_f }.should raise_error(RangeError)
  end

  it "raises a RangeError when m < 0 and the Integer argument is not in Fixnum range" do
    lambda { 3 >> -(fixnum_max() + 1) }.should raise_error(RangeError)
  end

  it "raises a RangeError when m < 0 the Float argument is not in Fixnum range" do
    lambda { 3 >> -(fixnum_max() + 1).to_f }.should raise_error(RangeError)
  end
end
