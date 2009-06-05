require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/update'

describe "Hash#update" do
  it_behaves_like(:hash_update, :update)
end
