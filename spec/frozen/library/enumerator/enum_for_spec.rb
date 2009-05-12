require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/enum_for'

describe "#enum_for" do
  it_behaves_like :enumerator_enum_for, :enum_for
end
