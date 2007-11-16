require 'rubygems'
require 'redcloth'
require 'erb'

section_template = ERB.new(File.read('section_template.html.erb'),nil,'<>')

Dir.glob("*.textile").each do |file|
  if ARGV.empty? or ARGV.include? file
    begin
      html_file = file.chomp('.textile') << '.html'
      red = RedCloth.new File.read(file)
      File.open(html_file, 'w') do |f|
        html = red.to_html
        f.puts section_template.result(binding)
      end
    rescue Exception => ex
      puts "An exception occurred while processing section '#{section}'"
      raise ex
    end
  end
end
