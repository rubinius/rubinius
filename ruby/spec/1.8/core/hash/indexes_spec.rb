require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/values_at_spec'

describe "Hash#indexes" do
  old, $VERBOSE = $VERBOSE, nil
  it_behaves_like(:hash_values_at, :indexes)
  $VERBOSE = old
end
