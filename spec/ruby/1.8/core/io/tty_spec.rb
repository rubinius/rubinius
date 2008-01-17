require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/tty_shared.rb'

describe "IO#tty?" do
  it_behaves_like :io_tty, :tty?
end
