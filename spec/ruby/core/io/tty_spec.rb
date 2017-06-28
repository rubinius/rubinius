require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/tty', __FILE__)

describe "IO#tty?" do
  it_behaves_like :io_tty, :tty?
end

describe "IO#ttyname" do
  with_tty do
    it "returns the name of the STDOUT tty" do
      io = $stdout
      io.ttyname.should =~ Regexp.new('/dev/')
    end
  end
end
