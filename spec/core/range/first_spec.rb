require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/begin_spec'

describe "Range#first" do
  it_behaves_like(@range_begin, :first)
end
