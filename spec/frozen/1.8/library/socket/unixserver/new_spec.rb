require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "UNIXServer.new" do
  it_behaves_like :unixserver_new, :new
end
