require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/eql.rb'

describe "BigDecimal#eql?" do
  it_behaves_like(:bigdecimal_eql, :eql?)
end
