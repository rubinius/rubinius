require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_i'

describe "Integer#floor" do
  it_behaves_like(:integer_to_i, :floor)
end
