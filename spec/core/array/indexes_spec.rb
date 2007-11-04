require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/indexes'

old, $VERBOSE = $VERBOSE, nil
describe "Array#indexes" do
  it_behaves_like(:array_indexes, :indexes)
end
$VERBOSE = old
