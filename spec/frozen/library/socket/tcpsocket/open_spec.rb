require File.dirname(__FILE__) + '/shared/new'

describe "TCPSocket.open" do
  it_behaves_like :tcpsocket_new, :open
end
