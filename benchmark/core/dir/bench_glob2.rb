require 'benchmark'
require 'benchmark/ips'

dirs = %w!
ZenTest-4.4.2
abstract-1.0.0
actionmailer-2.3.5
actionmailer-3.0.4
actionmailer-3.0.5
actionpack-2.3.5
actionpack-3.0.4
actionpack-3.0.5
activemodel-3.0.4
activemodel-3.0.5
activerecord-2.3.5
activerecord-3.0.4
activerecord-3.0.5
activeresource-2.3.5
activeresource-3.0.4
activeresource-3.0.5
activesupport-2.3.5
activesupport-3.0.4
activesupport-3.0.5
addressable-2.2.2
addressable-2.2.4
arel-2.0.8
arel-2.0.9
bcrypt-ruby-2.1.3
bcrypt-ruby-2.1.4
builder-2.1.2
builder-3.0.0
bundler-1.0.10
bundler-1.0.3
bundler-1.0.7
cheat-1.3.0
classifier-1.3.3
columnize-0.3.2
daemon_controller-0.2.6
devise-1.1.5
diff-lcs-1.1.2
directory_watcher-1.3.2
dummy-0.9.1
dummy_data-0.9.1
erubis-2.6.6
escape_utils-0.1.9
faker-0.3.1
fast-stemmer-1.0.0
fastthread-1.0.7
ffi-0.6.3
file-tail-1.0.5
git-1.2.5
highline-1.6.1
horo-1.0.2
hpricot-0.8.4
i18n-0.4.1
i18n-0.4.2
i18n-0.5.0
jekyll-0.8.0
jeweler-1.5.2
json-1.4.6
kgio-2.0.0
kramdown-0.12.0
libxml-ruby-1.1.4
liquid-2.2.2
mail-2.2.12
mail-2.2.15
mail-2.2.9.1
maruku-0.6.0
memcache-client-1.8.5
mime-types-1.16
mkrf-0.2.3
mocha-0.9.8
mocha-0.9.9
mustache-0.99.2
nokogiri-1.4.3.1
nokogiri-1.4.4
paperclip-2.3.8
passenger-3.0.2
pcap-0.7.1
polyglot-0.3.1
qtbindings-4.6.3.2
rack-1.0.1
rack-1.2.1
rack-1.2.2
rack-cache-0.5.3
rack-cache-purge-0.0.2
rack-mount-0.6.13
rack-mount-0.6.14
rack-test-0.5.6
rack-test-0.5.7
rails-2.3.5
rails-3.0.4
rails-3.0.5
railties-3.0.4
railties-3.0.5
rainbows-2.0.0
rbench-0.2.3
rbx-linecache-0.43
rbx-linecache-1.0-universal-rubinius-1.2
rbx-require-relative-0.0.3
rdiscount-1.6.8
rdoc-2.5.11
rest-client-1.6.1
ronn-0.7.3
rspec-1.3.1
rspec-2.5.0
rspec-core-2.5.1
rspec-expectations-2.5.0
rspec-mocks-2.5.0
rspec-rails-2.5.0
ruby-opengl-0.60.1
rubyfish-0.0.4
rubygems-update-1.5.2
rubysdl-2.1.1
simplecov-0.4.0
simplecov-html-0.4.3
sinatra-1.1.3
spruz-0.2.5
sqlite3-1.3.3
sqlite3-ruby-1.3.1
sqlite3-ruby-1.3.2
syntax-1.0.0
text-format-1.0.0
text-hyphen-1.0.0
thor-0.14.3
thor-0.14.4
thor-0.14.6
tilt-1.2.2
treetop-1.4.8
treetop-1.4.9
tzinfo-0.3.23
tzinfo-0.3.24
tzinfo-0.3.25
unicorn-3.0.0
vimgolf-0.1.1
warden-1.0.3
webrat-0.7.3
wirble-0.1.3
yajl-ruby-0.7.8
yard-0.6.4
zbatery-0.5.0
!

depth = %w!a b c d e f!

require 'tmpdir'
require 'fileutils'

base = File.join(Dir.tmpdir, "dir_glob_bench", *depth)

FileUtils.mkdir_p(base)

dirs.each do |f|
  path = File.join(base, f)
  Dir.mkdir(path) unless File.exists?(path)
end

Benchmark.ips do |x|

  parts = Dir["#{base}/*"]

  lib_dirs = ["lib"]
  suffixes = ["", ".rb", ".bundle"]

  x.report "Dir[]" do
    parts.each do |r|
      Dir["#{r}{lib}/blah{,.rb,.bundle}"]
    end
  end

  # x.report "File.file" do
    # parts.each do |r|
      # lib_dirs.each do |l|
        # base = "#{r}/#{l}/blah"
        # suffixes.each do |s|
          # path = "#{base}#{s}"
          # File.file? path
        # end
      # end
    # end
  # end
end
