require File.expand_path('../../../spec_helper', __FILE__)

describe "Float" do
  it "returns a float the smallest possible step greater than the receiver" do
    barely_positive = 0.0.next_float
    barely_positive.should eql 0.0.next_float

    barely_positive.should > 0.0
    barely_positive.should < barely_positive.next_float

    midpoint = barely_positive / 2
    [0.0, barely_positive].should include midpoint
  end

  it "steps directly between MAX and INFINITY" do
    (-Float::INFINITY).next_float.should eql -Float::MAX
    Float::MAX.next_float.should eql Float::INFINITY
  end

  it "steps directly between 1.0 and EPSILON + 1.0" do
    1.0.next_float.should eql Float::EPSILON + 1.0
  end

  it "steps directly between -1.0 and EPSILON/2 - 1.0" do
    (-1.0).next_float.should eql Float::EPSILON/2 - 1.0
  end

  it "reverses the effect of prev_float" do
    num = rand
    num.prev_float.next_float.should eql num
  end

  it "returns negative zero when stepping upward from just below zero" do
    x = 0.0.prev_float.next_float
    (1/x).should eql -Float::INFINITY
    x = (-0.0).prev_float.next_float
    (1/x).should eql -Float::INFINITY
    x.next_float.should > 0
  end

  it "returns NAN if NAN was the receiver" do
    Float::NAN.next_float.nan?.should eql true
  end
end
