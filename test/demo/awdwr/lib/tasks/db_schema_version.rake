namespace :db do
  desc "Prints the migration version"
  task :schema_version => :environment do
    puts ActiveRecord::Base.connection.select_value(
      'select version from schema_info')
  end
end
