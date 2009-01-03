require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/values_at_spec'

describe "Hash#indices" do
  it_behaves_like(:hash_values_at, :indices)
end
