require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../shared/recv_nonblock'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::BasicSocket#recv_nonblock" do
  it_behaves_like :socket_recv_nonblock, :recv_nonblock
end
