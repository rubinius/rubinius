require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/block_scanf.rb'
require 'scanf'

describe "IO#block_scanf" do
  it_behaves_like(:scanf_io_block_scanf, :block_scanf)
end
