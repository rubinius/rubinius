require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_i'

describe "Float#truncate" do
  it_behaves_like(:float_to_i, :truncate)
end
