require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/to_i_spec'

describe "Float#" do
  it_behaves_like(@float_to_i, :to_int)
end
