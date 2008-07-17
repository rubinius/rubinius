describe :time_day, :shared => true do
  it "returns the day of the month (1..n) for time" do
    with_timezone("CET", 1) do
      Time.at(0).send(@method).should == 1
    end
  end
end
