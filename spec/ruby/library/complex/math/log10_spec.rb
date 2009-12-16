require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/log10'

describe "Math#log10" do
  it_behaves_like :complex_math_log10, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:log10)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#log10!" do
    it_behaves_like :complex_math_log10_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:log10!)
    end
  end
end

describe "Math.log10" do
  it_behaves_like :complex_math_log10, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.log10!" do
    it_behaves_like :complex_math_log10_bang, :_, Math
  end
end
