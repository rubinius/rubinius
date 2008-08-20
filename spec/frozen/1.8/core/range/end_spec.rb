require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/end'

describe "Range#end" do
  it_behaves_like(:range_end, :end)
end
