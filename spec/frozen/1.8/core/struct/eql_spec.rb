require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/equal_value'

describe "Struct#eql?" do
  it_behaves_like(:struct_equal_value, :eql?)
end