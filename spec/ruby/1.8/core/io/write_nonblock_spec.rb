require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/write'

describe "IO#write_nonblock" do
  it_behaves_like(:io_write, :write_nonblock)
end
