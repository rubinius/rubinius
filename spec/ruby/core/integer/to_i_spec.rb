require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_i'

describe "Integer#to_i" do
  it_behaves_like(:integer_to_i, :to_i)
end
