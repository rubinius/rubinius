require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/include_spec'

describe "Range#===" do
  it_behaves_like(@range_include, :===)
end
