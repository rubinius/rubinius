require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/at'

describe "Tuple#[]" do
  it_behaves_like :tuple_at, :[]
end
