require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/tty'

describe "IO#isatty" do
  it_behaves_like :io_tty, :isatty
end
