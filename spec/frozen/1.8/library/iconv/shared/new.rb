require File.dirname(__FILE__) + '/initialize_exceptions.rb'

shared :iconv_new do |cmd|
  describe "Iconv.#{cmd}" do
    it "creates a new encoding converter" do
      obj = Iconv.send(cmd, "us-ascii", "us-ascii")
      begin
        obj.should be_kind_of(Iconv)
      ensure
        obj.close
      end
    end

    it_behaves_like :iconv_initialize_exceptions, cmd
  end
end
