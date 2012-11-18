# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#alphabetical?" do
    it "returns true for an ASCII alphabetical character" do
      char = new_character "a"
      char.alphabetical?.should be_true
    end

    it "returns true for a non-ASCII alphabetical character" do
      char = new_character "あ"
      char.alphabetical?.should be_true
    end

    it "returns false for a non-alphabetical character" do
      char = new_character "1"
      char.alphabetical?.should be_false
    end
  end
end
