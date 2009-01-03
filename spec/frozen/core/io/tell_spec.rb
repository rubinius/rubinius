require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/pos'

describe "IO#tell" do
  it_behaves_like(:io_pos, :tell)
end
