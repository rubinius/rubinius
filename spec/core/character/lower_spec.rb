# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#lower?" do
    it "returns true for a lower case character" do
      char = new_character "a"
      char.lower?.should be_true
    end

    it "returns false for an upper case character" do
      char = new_character "A"
      char.lower?.should be_false
    end
  end
end
