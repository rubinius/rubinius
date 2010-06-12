require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/clone', __FILE__)

describe "Rubinius::LookupTable#dup" do
  it_behaves_like :lookuptable_clone, :dup
end
