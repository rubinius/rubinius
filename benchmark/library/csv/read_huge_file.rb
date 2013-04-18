# this benchmark creates a csv file and then reads that with the csv library

require 'benchmark'
require 'benchmark/ips'
require 'csv'

CSV_SIZE       = 2_000_000 # around 86MB
CSV_FILE_PATH  = '/tmp/ruby_benchmark_csv_bench'
CSV_LINE       = "this is a string;72;1234567.9;another string\n"

File.open(CSV_FILE_PATH, 'w') do |f|
  1.upto(CSV_SIZE).each do
    f.write(CSV_LINE)
  end
end

Benchmark.ips do |x|
  x.report 'read using file' do |times|
    result = ''
    i = 0
    while i < times do
      File.open(CSV_FILE_PATH).each do |line|
        result = line
      end
      i += 1
    end
  end

  x.report 'read using csv' do |times|
    result = ''
    i = 0
    while i < times do
      CSV.foreach(CSV_FILE_PATH, :col_sep => ';') do |row|
        result = row
      end
      i += 1
    end
  end
end

File.delete(CSV_FILE_PATH)
