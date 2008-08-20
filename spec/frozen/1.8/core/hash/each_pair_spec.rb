require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/iteration'

describe "Hash#each_pair" do
  it "processes all pairs, yielding two arguments: key and value" do
    all_args = []

    h = {1 => 2, 3 => 4}
    h2 = h.each_pair { |*args| all_args << args }
    h2.should equal(h)

    all_args.should == [[1, 2], [3, 4]]
  end

  it_behaves_like(:hash_iteration_method, :each_pair)
  it_behaves_like(:hash_iteration_modifying, :each_pair)
  it_behaves_like(:hash_iteration_no_block, :each_pair)
end
