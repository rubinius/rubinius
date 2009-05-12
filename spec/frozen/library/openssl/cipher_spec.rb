require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'
require 'openssl'

describe "OpenSSL::Cipher's CipherError" do
  ruby_version_is "" ... "1.8.7" do
    it "exists under OpenSSL namespace" do
      OpenSSL.should have_constant :CipherError
    end
  end

  ruby_version_is "1.8.7" do
    it "exists under OpenSSL::Cipher namespace" do
      OpenSSL::Cipher.should have_constant :CipherError
    end
  end
end
