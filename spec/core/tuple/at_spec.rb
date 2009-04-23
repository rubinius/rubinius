require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/at'

describe "Rubinius::Tuple#at" do
  it_behaves_like :tuple_at, :at
end
