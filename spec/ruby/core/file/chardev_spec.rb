require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/chardev'

describe "File.chardev?" do
  it_behaves_like :file_chardev, :chardev?, File
end
