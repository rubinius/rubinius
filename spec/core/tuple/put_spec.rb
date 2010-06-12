require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/put', __FILE__)

describe "Rubinius::Tuple#put" do
  it_behaves_like :tuple_put, :put
end
