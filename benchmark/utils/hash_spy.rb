require File.dirname(__FILE__) + '/class_spy'

methods = [
  :==,
  :[]=,
  :[],
  :fetch,
  :key?,
  :member?,
  :include?,
  :has_key?,
  :keys,
  :values,
  :each,
  :each_pair,
  :delete
]

spy = ClassSpy.new Hash, methods
spy.store_objects

def spy.finish
  mixed_keys = 0
  key_types  = data_entry

  each_object do |id, hash|
    last_key = nil
    hash.class_spy_keys.each do |key|
      if last_key
        unless key.class.equal? last_key.class
          mixed_keys += 1
          last_key = key
        end
      else
        last_key = key
      end

      increment key_types, key.class.name
    end
  end

  puts "\nKey class                   Number"
  puts "----------------------------------"
  key_types.class_spy_sort.class_spy_each do |name, number|
    printf "%-25s %8d\n", name, number
  end

  if Rubinius::RUBY_CONFIG["rbx.hash_spy.max_object"]
    require 'pp'

    puts "\nMaximum objects"
    puts "---------------"

    if @size and @size.max > 0
      puts "Hash size"
      pp @size.max_object
      puts "---------------"
    end

    @methods.class_spy_each do |name, stat|
      if stat.max > 0
        puts "Method: #{name}"
        pp stat.max_object
        puts "---------------"
      end
    end
  end

  puts "\n#{mixed_keys} instances with mixed keys\n"
end
