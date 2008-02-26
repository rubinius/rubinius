require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/equal'

describe "Array#==" do
  it_behaves_like(:array_equal, :==)
end

# === behaves the same as ==
describe "Array#===" do
  it_behaves_like(:array_equal, :===)
end
