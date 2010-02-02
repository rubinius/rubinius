describe :regexp_match, :shared => true do
  it "returns nil if there is no match" do
    /xyz/.send(@method,"abxyc").should be_nil
  end

  it "returns nil if the object is nil" do
    /\w+/.send(@method, nil).should be_nil
  end
end
