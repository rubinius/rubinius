describe :time_now, :shared => true do
  it "creates a time based on the current system time" do
    unless `which date` == ""
      Time.__send__(@method).to_i.should.be_close(`date +%s`.to_i,1)
    end
  end
end
