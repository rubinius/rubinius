require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "UNIXServer.open" do
  it_behaves_like :unixserver_new, :open
end
