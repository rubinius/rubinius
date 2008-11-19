require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#<<" do
  it "writes an object to the IO stream" do
     lambda { 
      $stderr << "Oh noes, an error!"
    }.should output_to_fd("Oh noes, an error!", $stderr)
  end
  
  it "calls #to_s on the object to print it" do
     lambda { 
       $stderr << 1337
    }.should output_to_fd("1337", $stderr)
   end

  it "raises an error if the stream is closed" do
    io = IOSpecs.closed_file
    lambda { io << "test" }.should raise_error(IOError)
  end
end
