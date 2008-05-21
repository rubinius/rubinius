require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/asin'

describe "Math#asin" do
  it_behaves_like :complex_math_asin, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("asin")
  end
end

describe "Math#asin!" do
  it_behaves_like :complex_math_asin_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("asin!")
  end
end

describe "Math.asin" do
  it_behaves_like :complex_math_asin, Math
end

describe "Math.asin!" do
  it_behaves_like :complex_math_asin_bang, Math
end
