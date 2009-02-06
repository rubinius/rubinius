require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/asin'

describe "Math#asin" do
  it_behaves_like :complex_math_asin, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:asin)
  end
end

describe "Math#asin!" do
  it_behaves_like :complex_math_asin_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:asin!)
  end
end

describe "Math.asin" do
  it_behaves_like :complex_math_asin, :_, Math
end

describe "Math.asin!" do
  it_behaves_like :complex_math_asin_bang, :_, Math
end
