require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../shared/recv_nonblock'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::BasicSocket#recv_nonblock" do
  it_behaves_like(:recv_nonblock, "BasicSocket", :recv_nonblock)
end
