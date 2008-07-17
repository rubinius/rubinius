describe :time_gmt_offset, :shared => true do
  it "returns the offset in seconds between the timezone of time and UTC" do
    with_timezone("AST", 3) do
      Time.new.send(@method).should == 10800
    end
  end
end
