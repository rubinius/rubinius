require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/setuid'

describe "FileTest.setuid?" do
  it_behaves_like :file_setuid, :setuid?, FileTest
end
