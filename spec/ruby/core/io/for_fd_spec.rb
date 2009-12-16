require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "IO.for_fd" do
  it_behaves_like :io_new, :for_fd
end
