require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#fsync" do
   it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.fsync }.should raise_error(IOError)
  end

  it "writes the buffered data to permanent storage" do
    ret = ""
    File.open(tmp("fsync-specs"), "w") do |f|
      f.write "one hit wonder"
      lambda { 
        ret = f.fsync
      }.should_not raise_error
    end

    # it returns nil on unsupported systems
    [0, nil].include?(ret).should == true

    File.unlink(tmp("fsync-specs"))
  end
  
end
