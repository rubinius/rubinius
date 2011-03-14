require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  array = %w(Lorem ipsum singulis assentior ei pro ius eu choro verterem his dicta possim epicurei at Atomorum sadipscing ne cum sit ex hinc eros tation Eu mea oblique indoctum sed nostro mandamus recteque ut Ei usu labitur consequuntur Cu postulant consulatu reprimique vim his debet mucius platonem ne mucius facilis scriptorem vim ei)

  x.report "sort up" do |times|
    i = 0
    while i < times
      array.sort
      i += 1
    end
  end

  x.report "array sort! up" do |times|
    i = 0
    while i < times
      array2 = array.dup
      array2.sort!
      i += 1
    end
  end

  x.report "sort up and reverse!" do |times|
    i = 0
    while i < times
      array.sort.reverse!
      i += 1
    end
  end

  x.report "sort! up and reverse!" do |times|
    i = 0
    while i < times
      array2 = array.dup
      array2.sort!.reverse!
      i += 1
    end
  end

  x.report "sort down with block" do |times|
    i = 0
    while i < times
      array.sort { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "sort! down with block" do |times|
    i = 0
    while i < times
      array2 = array.dup
      array2.sort! { |a,b| b <=> a }
      i += 1
    end
  end

end
