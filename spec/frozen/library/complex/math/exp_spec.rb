require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/exp'

describe "Math#exp" do
  it_behaves_like :complex_math_exp, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:exp)
  end
end

describe "Math#exp!" do
  it_behaves_like :complex_math_exp_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:exp!)
  end
end

describe "Math.exp" do
  it_behaves_like :complex_math_exp, :_, Math
end

describe "Math.exp!" do
  it_behaves_like :complex_math_exp_bang, :_, Math
end
