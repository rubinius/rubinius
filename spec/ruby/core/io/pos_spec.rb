require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/pos'

describe "IO#pos" do
  it_behaves_like :io_pos, :pos
end

describe "IO#pos=" do
  it_behaves_like :io_set_pos, :pos=
end

