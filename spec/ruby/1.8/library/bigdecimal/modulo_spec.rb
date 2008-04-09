require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/modulo'

describe "BigDecimal#%" do
  it_behaves_like(:bigdecimal_modulo, :%)
end

describe "BigDecimal#modulo" do
  it_behaves_like(:bigdecimal_modulo, :modulo)
end
