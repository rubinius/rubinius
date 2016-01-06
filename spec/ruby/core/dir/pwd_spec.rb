# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)
require File.expand_path('../shared/pwd', __FILE__)

describe "Dir.pwd" do
  before :all do
    DirSpecs.create_mock_dirs
  end

  after :all do
    DirSpecs.delete_mock_dirs
  end

  it_behaves_like :dir_pwd, :pwd
end

describe "Dir.pwd" do
  before :all do
    @name = tmp("あ")
  end

  after :all do
    rm_r @name
  end

  it "correctly displays dirs with unicode characters in them" do
    Dir.mkdir @name

    Dir.chdir @name do
      Dir.pwd.should == @name
    end
  end
end
