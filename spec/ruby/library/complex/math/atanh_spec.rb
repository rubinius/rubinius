require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/atanh'

describe "Math#atanh" do
  it_behaves_like :complex_math_atanh, :atanh, IncludesMath.new
  it_behaves_like :complex_math_atanh_complex, :atanh, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:atanh)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#atanh!" do
    it_behaves_like :complex_math_atanh, :atanh!, IncludesMath.new
    it_behaves_like :complex_math_atanh_bang, :atanh!, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:atanh!)
    end
  end
end

describe "Math.atanh" do
  it_behaves_like :complex_math_atanh, :atanh, Math
  it_behaves_like :complex_math_atanh_complex, :atanh, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.atanh!" do
    it_behaves_like :complex_math_atanh, :atanh!, Math
    it_behaves_like :complex_math_atanh_bang, :atanh!, Math
  end
end
