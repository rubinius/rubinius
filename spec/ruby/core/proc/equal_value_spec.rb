require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/equal', __FILE__)

describe "Proc#==" do
  ruby_version_is ""..."2.0" do
    it_behaves_like(:proc_equal, :==)
  end

  ruby_version_is "2.0" do
    it_behaves_like(:proc_equal_undefined, :==)
  end
end
