require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/put', __FILE__)

describe "Rubinius::Tuple#[]=" do
  it_behaves_like :tuple_put, :[]=
end
