require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/kind_of'

describe "Object#kind_of?" do
  it_behaves_like(:object_kind_of , :kind_of?)
end
