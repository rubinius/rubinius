require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_i'

describe "Integer#truncate" do
  it_behaves_like(:integer_to_i, :truncate)
end
