require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_int'
require 'bigdecimal'


describe "BigDecimal#to_int" do
  it_behaves_like(:bigdecimal_to_int, :to_int)
end
