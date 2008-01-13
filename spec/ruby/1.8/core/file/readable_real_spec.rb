require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/readable_real'

describe "File.readable_real?" do
  it_behaves_like :file_readable_real, :readable_real?, File
  it_behaves_like :file_readable_real_missing, :readable_real?, File
end
