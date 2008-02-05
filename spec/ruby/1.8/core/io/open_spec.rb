require "#{File.dirname __FILE__}/../../spec_helper"
require "#{File.dirname __FILE__}/shared/new_shared"
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO.open" do
  it_behaves_like :io_new, :open
end

describe "IO.open" do
  before :each do
    @file_name = File.dirname(__FILE__) + '/fixtures/gets.txt'
  end

  it "raises IOError on closed stream" do
    lambda { IO.open(IOSpecs.closed_file.fileno, 'w') }.should raise_error(IOError)
  end

  it "with a block propagates non-StandardErrors produced by close" do
    lambda {
      File.open(@file_name, 'r') do |f|
        IO.open(f.fileno, 'r') do |io|
          def io.close
            raise Exception
          end
        end
      end
    }.should raise_error(Exception)
  end

  it "with a block swallows StandardErrors produced by close" do
    File.open(@file_name, 'r') do |f|
      IO.open(f.fileno, 'r') do |io|
        def io.close
          raise IOError
        end
      end
    end
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
