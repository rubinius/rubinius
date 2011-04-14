require File.expand_path('../../../spec_helper', __FILE__)
require 'uri/ftp'

describe "URI::FTP#path=" do
  before :each do
    @url = URI.parse('ftp://example.com')
  end

  ruby_version_is ""..."1.9" do
    it "requires a leading /" do
      lambda { @url.path = 'foo' }.should raise_error(URI::InvalidComponentError)
    end

    it "strips the leading /" do
      @url.path = '/foo'
      @url.path.should == 'foo'
    end
  end

  ruby_version_is "1.9" do
    it "does not require a leading /" do
      @url.path = 'foo'
      @url.path.should == 'foo'
    end

    it "does not strip the leading /" do
      @url.path = '/foo'
      @url.path.should == '/foo'
    end
  end
end

describe "URI::FTP#path" do
  it "unescapes the leading /" do
    url = URI.parse('ftp://example.com/%2Ffoo')

    url.path.should == '/foo'
  end
end

describe "URI::FTP#to_s" do
  before :each do
    @url = URI.parse('ftp://example.com')
  end

  ruby_version_is ""..."1.9" do
    it "does not escape the leading /" do
      @url.path = '//foo'

      @url.to_s.should == 'ftp://example.com//foo'
    end
  end

  ruby_version_is "1.9" do
    it "escapes the leading /" do
      @url.path = '/foo'

      @url.to_s.should == 'ftp://example.com/%2Ffoo'
    end
  end
end
