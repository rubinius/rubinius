require 'timeout'
files = `find #{File.dirname(__FILE__)}/../../externals/rspec-0.7.5/lib -type f`.scan(/.+\.rb/)

results = files.map do |path|
  begin
    command = "shotgun/rubinius #{path}"
    output = Timeout::timeout(5) do
      `(#{command}) 2>&1`
    end
    [path.split('rspec-0.7.5/')[1], output]
  rescue Timeout::Error
    [path.split('rspec-0.7.5/')[1], 'Timed out (infinite loop?)']
  end
end

puts Marshal.dump(results)
