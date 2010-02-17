require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../fixtures/math/common'
require File.dirname(__FILE__) + '/../../../shared/math/atanh'
require File.dirname(__FILE__) + '/shared/atanh'
require 'complex'

describe "Math#atanh" do
  it_behaves_like :math_atanh_base, :atanh, IncludesMath.new
  it_behaves_like :complex_math_atanh_complex, :atanh, IncludesMath.new

  it_behaves_like :math_atanh_private, :atanh
end

ruby_version_is ""..."1.9" do
  describe "Math#atanh!" do
    it_behaves_like :math_atanh_base, :atanh!, IncludesMath.new
    it_behaves_like :math_atanh_no_complex, :atanh!, IncludesMath.new
    it_behaves_like :complex_math_atanh_no_complex, :atanh!, IncludesMath.new

    it_behaves_like :math_atanh_private, :atanh!
  end
end

describe "Math.atanh" do
  it_behaves_like :math_atanh_base, :atanh, Math
  it_behaves_like :complex_math_atanh_complex, :atanh, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.atanh!" do
    it_behaves_like :math_atanh_base, :atanh!, Math
    it_behaves_like :math_atanh_no_complex, :atanh!, IncludesMath.new
    it_behaves_like :complex_math_atanh_no_complex, :atanh!, Math
  end
end
