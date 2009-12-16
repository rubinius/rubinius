require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/power'

describe "BigDecimal#power" do
  it_behaves_like(:bigdecimal_power, :power)
end
