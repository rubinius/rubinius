require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/has_key', __FILE__)

describe "Rubinius::LookupTable#has_key?" do
  it_behaves_like :lookuptable_has_key, :has_key?
end
