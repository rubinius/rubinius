require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes.rb', __FILE__)
require File.expand_path('../shared/concat.rb', __FILE__)

describe "String#concat" do
  it_behaves_like :string_concat, :concat
end
