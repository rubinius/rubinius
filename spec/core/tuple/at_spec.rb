require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/at', __FILE__)

describe "Rubinius::Tuple#at" do
  it_behaves_like :tuple_at, :at
end
