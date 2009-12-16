require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/common'

describe "Logger#close" do
  before :each do
    @path = tmp("test_log.log")
    @log_file = File.open(@path, "w+")
    @logger = Logger.new(@path)
  end

  after :each do
    @log_file.close unless @log_file.closed?
    File.unlink(@path) if File.exists?(@path)
  end

  it "closes the logging device" do
    @logger.close
    lambda { @logger.add(nil, "Foo") }.should raise_error(IOError)
  end

  it "fails when called on a closed device" do
    @logger.close
    lambda { @logger.close }.should raise_error(IOError)
  end
end
