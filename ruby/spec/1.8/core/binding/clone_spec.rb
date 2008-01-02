require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/clone'

describe "Binding#clone" do
  it_behaves_like(:binding_clone, :clone)
end
