require 'timeout'
RSPEC_PATH = File.expand_path(File.dirname(__FILE__) + '/../../externals/rspec-1.0.5/lib')
files = 
`find #{RSPEC_PATH} -type f`.scan(/^[^.].+?\.rb$/).map {|f| f.split('rspec-1.0.5/lib/')[1].split('.rb')[0] }

results = files.map do |path|
  begin
    command = "shotgun/rubinius -e \"$:.unshift('#{RSPEC_PATH}'); require '#{path}'\""
    output = Timeout::timeout(20) do
      `(#{command}) 2>&1`
    end
    [path, output]
  rescue Timeout::Error
    [path, 'Timed out.']
  end
end

puts Marshal.dump(results)
