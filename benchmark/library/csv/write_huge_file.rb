# this benchmark writes a csv file first using file, then csv

require 'benchmark'
require 'benchmark/ips'
require 'csv'

CSV_SIZE       = 2_000_000 # around 83MB
CSV_FILE_PATH  = '/tmp/ruby_benchmark_csv_bench'
CSV_LINE       = "this is a string;72;1234567.9;another string\n"
CSV_LINE_ARRAY = ['this is a string', '72', '1234567.9', 'another string']

Benchmark.ips do |x|
  x.report "write using File" do |times|
    i = 0
    while i < times do
      File.open(CSV_FILE_PATH, 'w') do |f|
        1.upto(CSV_SIZE).each do
          f.write(CSV_LINE)
        end
      end
      i += 1
    end
  end

  x.report 'write using CSV' do |times|
    i = 0
    while i < times do
      CSV.open(CSV_FILE_PATH, 'w') do |f|
        1.upto(CSV_SIZE).each do
          f << CSV_LINE_ARRAY
        end
      end
      i += 1
    end
  end
end

File.delete(CSV_FILE_PATH)
