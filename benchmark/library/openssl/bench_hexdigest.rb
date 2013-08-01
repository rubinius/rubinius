require 'digest/sha1'
require 'openssl'
require 'openssl/digest'
require 'benchmark'
require 'benchmark/ips'

input = 'hello, world!' * 100

Benchmark.ips do |bench|
  bench.report 'Digest SHA1' do |times|
    digest = Digest::SHA1.new
    i      = 0

    while i < times
      digest.hexdigest(input)
      i += 1
    end
  end

  bench.report 'OpenSSL SHA1' do |times|
    digest = OpenSSL::Digest::SHA1.new
    i      = 0

    while i < times
      digest.hexdigest(input)
      i += 1
    end
  end
end
