require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/has_key'

describe "Rubinius::LookupTable#include?" do
  it_behaves_like :lookuptable_has_key, :include?
end
