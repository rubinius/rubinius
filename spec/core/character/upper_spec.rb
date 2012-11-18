# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#upper?" do
    it "returns true for an upper case character" do
      char = new_character "A"
      char.upper?.should be_true
    end

    it "returns false for a lower case character" do
      char = new_character "a"
      char.upper?.should be_false
    end
  end
end
