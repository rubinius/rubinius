require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require File.dirname(__FILE__) + '/../shared/socketpair'

describe "Socket#pair" do
  it_behaves_like :socket_socketpair, :pair
end
