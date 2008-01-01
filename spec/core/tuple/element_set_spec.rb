require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/put'

describe "Tuple#[]=" do
  it_behaves_like :tuple_put, :[]=
end
