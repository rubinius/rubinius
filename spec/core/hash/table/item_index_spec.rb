require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_hamt do
  describe "Hash::Table#item_index" do
    it "returns the least significant six bits of the passed value" do
      state = Hash::State.new
      table = Hash::Table.new state

      [ [ table, 64,    0],
        [ table, 65,    1],
        [ table, 66,    2],
        [ table, 68,    4],
        [ table, 72,    8],
        [ table, 80,   16],
        [ table, 96,   32],
        [ table, 127,  63]
      ].should be_computed_by(:item_index)
    end
  end
end
