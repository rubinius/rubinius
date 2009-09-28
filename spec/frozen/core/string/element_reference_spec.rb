require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'
require File.dirname(__FILE__) + '/shared/slice.rb'

describe "String#[]" do
  it_behaves_like :string_slice, :[]
end

describe "String#[] with index, length" do
  it_behaves_like :string_slice_index_length, :[]
end

describe "String#[] with Range" do
  it_behaves_like :string_slice_range, :[]
end

describe "String#[] with Regexp" do
  it_behaves_like :string_slice_regexp, :[]
end

describe "String#[] with Regexp, index" do
  it_behaves_like :string_slice_regexp_index, :[]
end

ruby_version_is "1.9" do
  describe "String#[] with Regexp, group" do
    it_behaves_like :string_slice_regexp_group, :[]
  end
end

describe "String#[] with String" do
  it_behaves_like :string_slice_string, :[]
end
