require File.expand_path('../../fixtures/classes', __FILE__)

describe :time_now, :shared => true do
  platform_is_not :windows do
    it "creates a time based on the current system time" do
      unless `which date` == ""
        Time.send(@method).to_i.should be_close(`date +%s`.to_i, 2)
      end
    end
  end

  it "creates a subclass instance if called on a subclass" do
    TimeSpecs::SubTime.send(@method).should be_an_instance_of(TimeSpecs::SubTime)
  end
end
