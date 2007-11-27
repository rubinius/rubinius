require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/visibility'

describe "Module#protected" do
  it_behaves_like :set_method_visibility, :protected
end

