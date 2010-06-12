require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/has_key', __FILE__)

describe "Rubinius::LookupTable#include?" do
  it_behaves_like :lookuptable_has_key, :include?
end
