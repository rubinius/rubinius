require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/asin'

describe "Math#asin" do
  it_behaves_like :complex_math_asin, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:asin)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#asin!" do
    it_behaves_like :complex_math_asin_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:asin!)
    end
  end
end

describe "Math.asin" do
  it_behaves_like :complex_math_asin, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.asin!" do
    it_behaves_like :complex_math_asin_bang, :_, Math
  end
end
