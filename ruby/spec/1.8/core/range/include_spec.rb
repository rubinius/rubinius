require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/include'

describe "Range#include?" do
  it_behaves_like(:range_include, :include?)
end
