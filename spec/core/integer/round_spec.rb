require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/to_i_spec'

describe "Integer#round" do
  it_behaves_like(@integer_to_i, :round)
end
