require 'digest/sha1'
require 'openssl'
require 'openssl/digest'
require 'benchmark'

amount = 1_000_000
input  = 'hello, world!'

Benchmark.bmbm(40) do |bench|
  bench.report 'Digest::SHA1' do
    digest = Digest::SHA1.new

    amount.times do
      digest.hexdigest(input)
    end
  end

  bench.report 'OpenSSL::Digest::SHA1' do
    digest = OpenSSL::Digest::SHA1.new

    amount.times do
      digest.hexdigest(input)
    end
  end
end
