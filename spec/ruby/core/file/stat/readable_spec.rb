require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/readable'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#readable?" do
  it_behaves_like :file_readable, :readable?, FileStat
end
