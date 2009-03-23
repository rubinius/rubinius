require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'
require File.dirname(__FILE__) + '/shared/update'

describe "Hash#update" do
  it_behaves_like(:hash_update, :update)

  it_behaves_like(:hash_iteration_method, :update)

  compliant_on :rubinius do
    it_behaves_like(:hash_iteration_modifying, :update)
  end
end
