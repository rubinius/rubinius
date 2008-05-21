require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/value'

describe "Hash#has_value?" do
  it_behaves_like(:hash_value_p, :has_value?)
end

