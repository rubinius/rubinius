require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/slice'

describe "Array#[]" do
  it_behaves_like(:array_slice, :[])
end
