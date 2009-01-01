require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/closed'
require File.dirname(__FILE__) + '/shared/pos'

describe "Dir#pos" do
  it_behaves_like :dir_pos, :pos
end

describe "Dir#pos" do
  it_behaves_like :dir_closed, :pos
end

describe "Dir#pos=" do
  it_behaves_like :dir_pos_set, :pos=
end
