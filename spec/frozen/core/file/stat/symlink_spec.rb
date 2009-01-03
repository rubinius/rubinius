require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/symlink'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#symlink?" do
  it_behaves_like :file_symlink, :symlink?, FileStat
end
