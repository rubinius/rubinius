require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Thread#abort_on_exception" do
  before :each do
    @thread = Thread.new { sleep }
  end

  after :each do
    @thread.kill
  end

  it "is false by default" do
    @thread.abort_on_exception.should == false
  end

  it "is changeable to true or false" do
    @thread.abort_on_exception = true
    @thread.abort_on_exception.should == true
    @thread.abort_on_exception = false
    @thread.abort_on_exception.should == false
  end
end

describe "Thread#abort_on_exception=" do
  describe "when enabled and the thread dies due to an exception" do
    before :each do
      @saved_stderr = $stderr
      $stderr = IOStub.new
    end

    after :each do
      $stderr = @saved_stderr
    end

    ruby_version_is ""..."1.9.2" do
      it "causes the main thread to raise a SystemExit" do
	go = false
	t = Thread.new { 1 until go; raise }
	lambda {t.abort_on_exception = true; go = true; t.join}.should raise_error(SystemExit)
      end
    end

    ruby_version_is "1.9.2" do
      it "causes the main thread to raise that exception" do
	go = false
	t = Thread.new { 1 until go; raise }
	lambda {t.abort_on_exception = true; go = true; t.join}.should raise_error(RuntimeError)
      end
    end
  end
end

describe "Thread.abort_on_exception" do
  it "is false by default" do
    Thread.abort_on_exception.should == false
  end

  it "is changeable to true or false" do
    Thread.abort_on_exception = true
    Thread.abort_on_exception.should == true
    Thread.abort_on_exception = false
    Thread.abort_on_exception.should == false
  end
end

describe "Thread.abort_on_exception=" do
  describe "when enabled and a non-main thread dies due to an exception" do
    before :each do
      @abort_on_exception = Thread.abort_on_exception
      Thread.abort_on_exception = true
      @saved_stderr = $stderr
      $stderr = IOStub.new
    end

    after :each do
      Thread.abort_on_exception = @abort_on_exception
      $stderr = @saved_stderr
    end

    ruby_version_is ""..."1.9.2" do
      it "causes the main thread to raise a SystemExit" do
	go = false
	t = Thread.new { 1 until go; raise }
	lambda { go = true; t.join }.should raise_error(SystemExit)
      end
    end

    ruby_version_is "1.9.2" do
      it "causes the main thread to raise that exception" do
	go = false
	t = Thread.new { 1 until go; raise }
	lambda { go = true; t.join }.should raise_error(RuntimeError)
      end
    end
  end
end
