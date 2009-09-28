require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do
  require File.dirname(__FILE__) + '/../../shared/enumerator/enum_for'
  require 'enumerator'

  describe "#to_enum" do
    it_behaves_like :enum_for, :enum_for
  end
end
