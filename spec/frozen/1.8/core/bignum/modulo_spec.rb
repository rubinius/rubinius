require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/modulo'

describe "Bignum#%" do
  it_behaves_like(:bignum_modulo, :%)
end

describe "Bignum#modulo" do
  it_behaves_like(:bignum_modulo, :modulo)
end
