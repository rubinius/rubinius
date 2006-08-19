
if dir = ARGV.shift
    Dir["#{dir}/test_*"].each do |f|
        require f
    end
else

    dir = File.dirname(File.expand_path(__FILE__))
    Dir["#{dir}/**/test_*"].each do |f|
      require f
    end
end
