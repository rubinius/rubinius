require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/random_bytes.rb'

describe "OpenSSL::Random#pseudo_bytes" do
  it_behaves_like :openssl_random_bytes, :pseudo_bytes
end
