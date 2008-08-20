require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/indexes'

describe "Array#indices" do
  it_behaves_like(:array_indexes, :indices)
end
