require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/to_s_spec'

describe "UnboundMethod#inspect" do
  it_behaves_like(@unboundmethod_to_s, :inspect)
end
