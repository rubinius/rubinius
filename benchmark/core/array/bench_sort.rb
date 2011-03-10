require 'benchmark'
require 'benchmark/ips'

array_o = %w(Lorem ipsum singulis assentior ei pro ius eu choro verterem his dicta possim epicurei at Atomorum sadipscing ne cum sit ex hinc eros tation Eu mea oblique indoctum sed nostro mandamus recteque ut Ei usu labitur consequuntur Cu postulant consulatu reprimique vim his debet mucius platonem ne mucius facilis scriptorem vim ei)


Benchmark.ips do |x|

  x.report "array sort up" do |times|
    i = 0
    while i < times
      array = array_o.dup
      array.sort
      i += 1
    end
  end

  x.report "array sort! up" do |times|
    i = 0
    while i < times
      array = array_o.dup
      array.sort!
      i += 1
    end
  end

  x.report "array sort up and reverse!" do |times|
    i = 0
    while i < times
      array = array_o.dup
      array.sort.reverse!
      i += 1
    end
  end

  x.report "array sort! up and reverse!" do |times|
    i = 0
    while i < times
      array = array_o.dup
      array.sort!.reverse!
      i += 1
    end
  end


  x.report "array sort down with block" do |times|
    i = 0
    while i < times
      array = array_o.dup
      array.sort { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "array sort! down with block" do |times|
    i = 0
    while i < times
      array = array_o.dup
      array.sort! { |a,b| b <=> a }
      i += 1
    end
  end

end

