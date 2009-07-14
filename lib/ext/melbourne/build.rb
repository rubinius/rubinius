extension do |e|
  e.name 'melbourne'
  e.files '*.c', '*.cpp'
  e.includes '.', '../../../vm/external_libs/libbstring',
             '../../../vm/external_libs/libmquark',
             '../../../vm/external_libs/libmpa',
             '../../../vm/external_libs/libcchash'

  e.pre_compile do
    if File.mtime("grammar.y") > File.mtime("grammar.cpp")
      system "bison -o grammar.cpp grammar.y"
    end
  end
end
