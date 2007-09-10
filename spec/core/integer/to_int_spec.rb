require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/to_i_spec'

describe "Integer#to_int" do
  it_behaves_like(integer_to_i, :to_int)
end
