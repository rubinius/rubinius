require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/to_s'

describe "Hash#inspect" do
  it_behaves_like :to_s, :inspect
end
