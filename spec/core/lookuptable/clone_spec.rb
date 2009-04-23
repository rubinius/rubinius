require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/clone'

describe "Rubinius::LookupTable#clone" do
  it_behaves_like :lookuptable_clone, :clone
end
