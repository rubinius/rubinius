require File.dirname(__FILE__) + '/shared/new'

describe "TCPSocket.new" do
  it_behaves_like :tcpsocket_new, :new
end
