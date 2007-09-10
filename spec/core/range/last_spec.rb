require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/end_spec'

describe "Range#last" do
  it_behaves_like(range_end, :last)
end
