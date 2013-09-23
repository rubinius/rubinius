require File.expand_path('../../../spec_helper', __FILE__)
require 'date'

ruby_version_is '1.9' do
  describe "Random#rand with a Range of Time" do
    before do
      @start = Time.new(2013, 1, 1)
      @stop  = Time.new(2014, 1, 1)
    end

    it "returns an element from the Range" do
      Random.new.rand(@start..@stop).should be_an_instance_of(Time)
    end

    it "returns an object that is a member of the Range" do
      value = Random.new.rand(@start..@stop)

      (value >= @start && value <= @stop).should == true
    end

    # This is put in place to ensure that #rand works when the date library is
    # not loaded. ruby_exe is used to run the code in an isolated/non-polluted
    # environment.
    it "creates a random Time value when Date/DateTime is not available" do
      script = <<-EOF
      start = Time.new(2013, 1, 1)
      stop  = Time.new(2014, 1, 1)
      value = Random.new.rand(start..stop)

      print value.class
      EOF

      ruby_exe(script, :escape => true).should == 'Time'
    end
  end

  describe "Random#rand with a Range of Date" do
    before do
      @start = Date.new(2013, 1, 1)
      @stop  = Date.new(2014, 1, 1)
    end

    it "returns an element from the Range" do
      Random.new.rand(@start..@stop).should be_an_instance_of(Date)
    end

    it "returns an object that is a member of the Range" do
      value = Random.new.rand(@start..@stop)

      (value >= @start && value <= @stop).should == true
    end
  end

  describe "Random#rand with a Range of DateTime" do
    before do
      @start = DateTime.new(2013, 1, 1, 12)
      @stop  = DateTime.new(2014, 1, 1, 12)
    end

    it "returns an element from the Range" do
      Random.new.rand(@start..@stop).should be_an_instance_of(DateTime)
    end

    it "returns an object that is a member of the Range" do
      value = Random.new.rand(@start..@stop)

      (value >= @start && value <= @stop).should == true
    end
  end
end
