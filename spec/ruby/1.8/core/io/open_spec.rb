require "#{File.dirname __FILE__}/../../spec_helper"
require "#{File.dirname __FILE__}/shared/new_shared"
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO.open" do
  it_behaves_like :io_new, :open
end

describe "IO.open" do
  it "raises IOError on closed stream" do
    lambda { IO.open(IOSpecs.closed_file.fileno, 'w') }.should raise_error(IOError)
  end
#  before :all do
#    @filename = "/tmp/rubinius-spec-io-new-#{$$}.txt"
#  end
#
#  after :all do
#    File.unlink @filename
#  end
#
#  before :each do
#    @file = File.open @filename, "w"
#  end
#
#  after :each do
#    # This should normally NOT be rescued
#    @file.close unless @file.closed? rescue nil
#  end
end
