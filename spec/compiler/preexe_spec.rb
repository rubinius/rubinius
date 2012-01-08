require File.expand_path('../../spec_helper', __FILE__)

describe "A Preexe node" do
  ruby_version_is ""..."1.9" do
    relates "BEGIN { 1 }" do
      compile do |g|
        g.push_literal Rubinius::Compiler::Runtime

        in_block_send :pre_exe, :none, nil, 0, false do |d|
          d.push 1
        end
      end
    end
  end

  ruby_version_is "1.9" do
    relates "BEGIN { 1 }" do
      compile do |g|
        g.push 1
      end
    end
  end
end
