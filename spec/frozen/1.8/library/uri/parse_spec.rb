require File.dirname(__FILE__) + '/../../spec_helper'
require 'uri'

def components(uri)
  result = {}
  uri.component.each do |component|
    result[component] = uri.send(component)
  end
  result
end

describe "URI.parse" do
  it "returns a URI::HTTP object when parsing an HTTP URI" do
    URI.parse("http://www.example.com/").should be_kind_of(URI::HTTP)
  end
  
  it "populates the components of a parsed URI::HTTP, setting the port to 80 by default" do
    components(URI.parse("http://user:pass@example.com/path/?query=val&q2=val2#fragment")).should == {
      :scheme => "http",
      :userinfo => "user:pass",
      :host => "example.com",
      :port => 80,
      :path => "/path/",
      :query => "query=val&q2=val2",
      :fragment => "fragment"
    }
  end

  it "parses out the port number of a URI, when given" do
    URI.parse("http://example.com:8080/").port.should == 8080    
  end
  
  it "returns a URI::HTTPS object when parsing an HTTPS URI" do
    URI.parse("https://important-intern-net.net").should be_kind_of(URI::HTTPS)
  end
  
  it "sets the port of a parsed https URI to 443 by default" do
    URI.parse("https://example.com/").port.should == 443
  end
  
  it "returns a URI::FTP object when parsing an FTP URI" do
    URI.parse("ftp://ruby-lang.org/").should be_kind_of(URI::FTP)
  end
  
  ruby_version_is "" ... "1.8.7" do
    it "populates the components of a parsed URI::FTP object" do
      components(URI.parse("ftp://anonymous@ruby-lang.org/pub/ruby/1.8/ruby-1.8.6.tar.bz2;type=i")).should == {
        :scheme => "ftp",
        :userinfo => "anonymous",
        :host => "ruby-lang.org",
        :port => 21,
        :path => "/pub/ruby/1.8/ruby-1.8.6.tar.bz2",
        :typecode => "i"
      }
    end
  end

  ruby_version_is "1.8.7" do
    it "populates the components of a parsed URI::FTP object" do
      components(URI.parse("ftp://anonymous@ruby-lang.org/pub/ruby/1.8/ruby-1.8.6.tar.bz2;type=i")).should == {
        :scheme => "ftp",
        :userinfo => "anonymous",
        :host => "ruby-lang.org",
        :port => 21,
        :path => "pub/ruby/1.8/ruby-1.8.6.tar.bz2",
        :typecode => "i"
      }
    end
  end

  it "returns a URI::LDAP object when parsing an LDAP URI" do
    #taken from http://www.faqs.org/rfcs/rfc2255.html 'cause I don't really know what an LDAP url looks like
    ldap_uris = %w{ ldap:///o=University%20of%20Michigan,c=US ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US?postalAddress ldap://host.com:6666/o=University%20of%20Michigan,c=US??sub?(cn=Babs%20Jensen) ldap://ldap.itd.umich.edu/c=GB?objectClass?one ldap://ldap.question.com/o=Question%3f,c=US?mail ldap://ldap.netscape.com/o=Babsco,c=US??(int=%5c00%5c00%5c00%5c04) ldap:///??sub??bindname=cn=Manager%2co=Foo ldap:///??sub??!bindname=cn=Manager%2co=Foo }
    ldap_uris.each do |ldap_uri|
      URI.parse(ldap_uri).should be_kind_of(URI::LDAP)
    end
  end
  
  it "populates the components of a parsed URI::LDAP object" do
    components(URI.parse("ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US?postalAddress?scope?filter?extensions")).should == {
      :scheme => "ldap",
      :host => "ldap.itd.umich.edu",
      :port => 389,
      :dn => "o=University%20of%20Michigan,c=US",
      :attributes => "postalAddress",
      :scope => "scope",
      :filter => "filter",
      :extensions => "extensions"
    }
  end
  
  it "returns a URI::MailTo object when passed a mailto URI" do
    URI.parse("mailto:spam@mailinator.com").should be_kind_of(URI::MailTo)
  end
  
  it "populates the components of a parsed URI::MailTo object" do
    components(URI.parse("mailto:spam@mailinator.com?subject=Discounts%20On%20Imported%20methods!!!&body=Exciting%20offer")).should == {
      :scheme => "mailto",
      :to => "spam@mailinator.com",
      :headers => [["subject","Discounts%20On%20Imported%20methods!!!"],
                   ["body", "Exciting%20offer"]]
    }
  end
  
  it "should return a URI::Genaric object when passed any other protocol URI" do
    #feel free to add a URI with your favorite protocol
    ["telnet://play.sdmud.org/", 
     "gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles", 
     "news:alt.religion.kibology",
     "git://github.com/brixen/rubyspec.git"].each do |uri|
      
      URI.parse(uri).should be_kind_of(URI::Generic)  
    end
  end
  
  
  #TODO: look for corner cases here
  #      test relative urls more
  #      test :registry and :opaque
  it "does its best to extract components from URI::Generic objects" do
    components(URI("scheme://userinfo@host/path?query#fragment")).should == {
      :scheme => "scheme",
      :userinfo => "userinfo",
      :host => "host",
      :port => nil,
      :path => "/path",
      :query => "query",
      :fragment => "fragment",
      :registry => nil,
      :opaque => nil
    }
  end
  
  def uri_to_ary(uri)
    uri.class.component.collect {|c| uri.send(c)}
  end
  
  it "conforms to the tests from MatzRuby's generic uri tests" do
    @url = 'http://a/b/c/d;p?q'
    @base_url = URI.parse(@url)
    
    # 0
    @base_url.should be_kind_of(URI::HTTP)

    exp = [
      'http', 
      nil, 'a', URI::HTTP.default_port, 
      '/b/c/d;p', 
      'q',
      nil
    ]
    ary = uri_to_ary(@base_url)
    ary.should == exp

    # 1
    url = URI.parse('ftp://ftp.is.co.za/rfc/rfc1808.txt')
    url.should be_kind_of(URI::FTP)

    ruby_version_is "" ... "1.8.7" do
      exp = [
        'ftp', 
        nil, 'ftp.is.co.za', URI::FTP.default_port, 
        '/rfc/rfc1808.txt', nil,
      ]
    end

    ruby_version_is "1.8.7" do
      exp = [
        'ftp', 
        nil, 'ftp.is.co.za', URI::FTP.default_port, 
        'rfc/rfc1808.txt', nil,
      ]
    end
    
    ary = uri_to_ary(url)
    ary.should == exp

    # 2
    url = URI.parse('gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles')
    url.should be_kind_of(URI::Generic)

    exp = [
      'gopher', 
      nil, 'spinaltap.micro.umn.edu', nil, nil,
      '/00/Weather/California/Los%20Angeles', nil,
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    ary.should == exp

    # 3
    url = URI.parse('http://www.math.uio.no/faq/compression-faq/part1.html')
    url.should be_kind_of(URI::HTTP)

    exp = [
      'http', 
      nil, 'www.math.uio.no', URI::HTTP.default_port, 
      '/faq/compression-faq/part1.html', 
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    ary.should == exp

    # 4
    url = URI.parse('mailto:mduerst@ifi.unizh.ch')
    url.should be_kind_of(URI::Generic)

    exp = [
      'mailto', 
      'mduerst@ifi.unizh.ch',
      []
    ]
    ary = uri_to_ary(url)
    ary.should == exp

    # 5
    url = URI.parse('news:comp.infosystems.www.servers.unix')
    url.should be_kind_of(URI::Generic)

    exp = [
      'news', 
      nil, nil, nil, nil, 
      nil, 'comp.infosystems.www.servers.unix',
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    ary.should == exp

    # 6
    url = URI.parse('telnet://melvyl.ucop.edu/')
    url.should be_kind_of(URI::Generic)

    exp = [
      'telnet', 
      nil, 'melvyl.ucop.edu', nil, nil, 
      '/', nil,
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    ary.should == exp

    # 7
    # reported by Mr. Kubota <em6t-kbt@asahi-net.or.jp>
    lambda { URI.parse('http://a_b:80/') }.should raise_error(URI::InvalidURIError)
    lambda { URI.parse('http://a_b/') }.should raise_error(URI::InvalidURIError)

    # 8
    # reported by m_seki
    uri = URI.parse('file:///foo/bar.txt')
    url.should be_kind_of(URI::Generic)
    uri = URI.parse('file:/foo/bar.txt')
    url.should be_kind_of(URI::Generic)

    # 9
    # [ruby-dev:25667]
    url = URI.parse('ftp://:pass@localhost/')
    url.user.should == ''
    url.password.should == 'pass'
    url.userinfo.should == ':pass'
    url = URI.parse('ftp://user@localhost/')
    url.user.should == 'user'
    url.password.should == nil
    url.userinfo.should == 'user'
    url = URI.parse('ftp://localhost/')
    url.user.should == nil
    url.password.should == nil
    url.userinfo.should == nil
  end
  
  it "conforms to MatzRuby's ldap tests" do
    url = 'ldap://ldap.jaist.ac.jp/o=JAIST,c=JP?sn?base?(sn=ttate*)'
    u = URI.parse(url)
    u.should be_kind_of(URI::LDAP)
    u.to_s.should == url
    u.dn.should == 'o=JAIST,c=JP'
    u.attributes.should == 'sn'
    u.scope.should == 'base'
    u.filter.should == '(sn=ttate*)'
    u.extensions.should == nil

    u.scope = URI::LDAP::SCOPE_SUB
    u.attributes = 'sn,cn,mail'
    u.to_s.should == 'ldap://ldap.jaist.ac.jp/o=JAIST,c=JP?sn,cn,mail?sub?(sn=ttate*)'
    u.dn.should == 'o=JAIST,c=JP'
    u.attributes.should == 'sn,cn,mail'
    u.scope.should == 'sub'
    u.filter.should == '(sn=ttate*)'
    u.extensions.should == nil

    # from RFC2255, section 6.
    urls = {
      'ldap:///o=University%20of%20Michigan,c=US' =>
      ['ldap', nil, URI::LDAP::DEFAULT_PORT, 
	'o=University%20of%20Michigan,c=US', 
	nil, nil, nil, nil],

      'ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US' =>
      ['ldap', 'ldap.itd.umich.edu', URI::LDAP::DEFAULT_PORT, 
	'o=University%20of%20Michigan,c=US', 
	nil, nil, nil, nil],

      'ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US?postalAddress' =>
      ['ldap', 'ldap.itd.umich.edu', URI::LDAP::DEFAULT_PORT, 
	'o=University%20of%20Michigan,c=US',
	'postalAddress', nil, nil, nil],

      'ldap://host.com:6666/o=University%20of%20Michigan,c=US??sub?(cn=Babs%20Jensen)' =>
      ['ldap', 'host.com', 6666, 
	'o=University%20of%20Michigan,c=US',
	nil, 'sub', '(cn=Babs%20Jensen)', nil],

      'ldap://ldap.itd.umich.edu/c=GB?objectClass?one' =>
      ['ldap', 'ldap.itd.umich.edu', URI::LDAP::DEFAULT_PORT, 
	'c=GB', 
	'objectClass', 'one', nil, nil],

      'ldap://ldap.question.com/o=Question%3f,c=US?mail' =>
      ['ldap', 'ldap.question.com', URI::LDAP::DEFAULT_PORT, 
	'o=Question%3f,c=US',
	'mail', nil, nil, nil],

      'ldap://ldap.netscape.com/o=Babsco,c=US??(int=%5c00%5c00%5c00%5c04)' =>
      ['ldap', 'ldap.netscape.com', URI::LDAP::DEFAULT_PORT, 
	'o=Babsco,c=US',
	nil, '(int=%5c00%5c00%5c00%5c04)', nil, nil],

      'ldap:///??sub??bindname=cn=Manager%2co=Foo' =>
      ['ldap', nil, URI::LDAP::DEFAULT_PORT, 
	'',
	nil, 'sub', nil, 'bindname=cn=Manager%2co=Foo'],

      'ldap:///??sub??!bindname=cn=Manager%2co=Foo' =>
      ['ldap', nil, URI::LDAP::DEFAULT_PORT, 
	'',
	nil, 'sub', nil, '!bindname=cn=Manager%2co=Foo'],
    }.each do |url, ary|
      u = URI.parse(url)
      uri_to_ary(u).should == ary
    end
  end
end