require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/modulo'

describe "BigDecimal#%" do
  it_behaves_like(:bigdecimal_modulo, :%)
  it_behaves_like(:bigdecimal_modulo_zerodivisionerror, :%)
end

describe "BigDecimal#modulo" do
  it_behaves_like(:bigdecimal_modulo, :modulo)
  it_behaves_like(:bigdecimal_modulo_zerodivisionerror, :modulo)
end
