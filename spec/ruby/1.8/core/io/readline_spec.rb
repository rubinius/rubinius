require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#readline" do
  @testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'

  it "raises EOFError on end of stream" do
    File.open(@testfile, 'r') do |f|
      lambda { loop { f.readline } }.should raise_error(EOFError)
    end
  end
end

