require File.expand_path('../shared/each_with_index', __FILE__)

describe "Enumerable#each_with_index" do
  it_behaves_like(:each_with_index, :enum_with_index)
end

