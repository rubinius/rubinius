require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/readable'

describe "FileTest.readable?" do
  it_behaves_like :file_readable, :readable?, FileTest
end
