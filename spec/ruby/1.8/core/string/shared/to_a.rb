shared :string_to_a do |cmd|
  describe "String##{cmd}" do
    it "returns an empty array for empty strings" do
      "".send(cmd).should == []
    end

    it "returns an array containing the string for non-empty strings" do
      "hello".send(cmd).should == ["hello"]
    end
  end
end
