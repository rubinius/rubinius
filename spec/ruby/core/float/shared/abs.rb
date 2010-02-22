require File.expand_path('../../../../spec_helper', __FILE__)

describe :float_abs, :shared => true do
  it "returns the absolute value" do
    -99.1.abs.should be_close(99.1, TOLERANCE)
    4.5.abs.should be_close(4.5, TOLERANCE)
    0.0.abs.should be_close(0.0, TOLERANCE)
  end
end
