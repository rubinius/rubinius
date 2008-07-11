require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "UNIXSocket.open" do
  it_behaves_like :new, :open
end
