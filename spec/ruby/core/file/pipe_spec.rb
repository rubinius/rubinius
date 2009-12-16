require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/pipe'

describe "File.pipe?" do
  it_behaves_like :file_pipe, :pipe?, File
end

describe "File.pipe?" do
  it "needs to be reviewed for spec completeness"
end
