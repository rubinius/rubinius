require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/begin'

describe "Range#begin" do
  it_behaves_like(:range_begin, :begin)
end
