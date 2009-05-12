require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/enum_for'

describe "#to_enum" do
  it_behaves_like :enumerator_enum_for, :enum_for
end
