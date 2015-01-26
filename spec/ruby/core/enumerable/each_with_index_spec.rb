require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/each_with_index', __FILE__)

describe "Enumerable#each_with_index" do
  it_behaves_like(:each_with_index, :each_with_index)
end
