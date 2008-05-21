require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/symlink'

describe "FileTest.symlink?" do
  it_behaves_like :file_symlink, :symlink?, FileTest
end
