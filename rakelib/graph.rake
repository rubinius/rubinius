task :graph, :exclude do |_, args|
  begin
    require 'graph'
  rescue LoadError
    abort 'install ZenHacks'
  end

  exclude = /#{args[:exclude]}/ if args[:exclude]

  graph = Graph.new

  Rake::Task.tasks.each do |task|
    next if exclude and task.name =~ exclude
    graph[task.name] = task.prerequisites
  end

  open 'rubinius_tasks.dot', 'w' do |io| io << graph end
end

