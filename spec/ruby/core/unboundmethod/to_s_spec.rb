require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/to_s'

describe "UnboundMethod#to_s" do
  it_behaves_like(:unboundmethod_to_s, :to_s)
end
