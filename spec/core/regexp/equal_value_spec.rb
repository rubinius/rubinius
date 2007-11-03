require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal_value'

describe "Regexp#==" do
  it_behaves_like(:regexp_eql, :==)
end
