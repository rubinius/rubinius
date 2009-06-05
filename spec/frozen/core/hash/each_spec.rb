require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'
require File.dirname(__FILE__) + '/shared/each'

describe "Hash#each" do
  it_behaves_like(:hash_each, :each)
  it_behaves_like(:hash_iteration_no_block, :each)
end
