require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_a'

describe "String#entries" do
  it_behaves_like :string_to_a, :entries
end
