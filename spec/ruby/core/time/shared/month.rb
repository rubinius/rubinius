describe :time_month, :shared => true do
  it "returns the month of the year" do
    Time.at(99999).send(@method).should == 1
  end
end
