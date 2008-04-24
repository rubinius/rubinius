require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#each_byte" do
  it "raises IOError on closed stream" do
    # each_byte must have a block in order to raise the Error.
    # MRI 1.8.7 returns enumerator if block is not provided.
    # See [ruby-core:16557].
    lambda { IOSpecs.closed_file.each_byte{} }.should raise_error(IOError)
  end
end
